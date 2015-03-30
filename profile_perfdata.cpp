#include "profile.h"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include <util/session.h>
#include <util/util.h>
#include <util/evsel.h>

const char perf_version_string[] = "perf in qprofiler";
int use_browser = -1;
extern "C" void test_attr__open(struct perf_event_attr *attr, pid_t pid, int cpu,
             int fd, int group_fd, unsigned long flags)
{}

namespace
{
    char const* get_sym_name(symbol* sym)
    {
        if (sym)
            return sym->name;
        else
            return "[unknown]";
    }

    char const* get_dso_name(map* m)
    {
        if (m)
            return m->dso->long_name;
        else
            return "[unknown]";
    }

    template <typename OnSample>
    void process_events(perf_session* session, OnSample on_sample)
    {
        struct profile_opening_tool : perf_tool
        {
            profile_opening_tool(OnSample on_sample)
                : perf_tool()
                , on_sample(on_sample)
            {
            }

            OnSample on_sample;
        };

        profile_opening_tool tool(on_sample);

        tool.sample       = [](perf_tool *that, perf_event *event,
                               perf_sample *sample,
                               perf_evsel *evsel, machine *machine)
        {
            profile_opening_tool* cthat = static_cast<profile_opening_tool*>(that);
            return cthat->on_sample(event, sample, evsel, machine);
        };

        tool.mmap         = perf_event__process_mmap;
        tool.mmap2        = perf_event__process_mmap2;
        tool.comm         = perf_event__process_comm;
        tool.exit         = perf_event__process_exit;
        tool.fork         = perf_event__process_fork;
        tool.attr         = perf_event__process_attr;
        tool.tracing_data = perf_event__process_tracing_data;
        tool.build_id     = perf_event__process_build_id;
        tool.ordered_samples = true;
        tool.ordering_requires_timestamps = true;

        perf_session__process_events(session, &tool);
    }
}

void profile::open_perf_data(const std::string &filename)
{
    samples.clear();
    if (symbol__init() < 0)
        throw std::runtime_error("perf init failed");

    perf_data_file file = {};
    file.path = filename.c_str();
    file.mode = PERF_DATA_MODE_READ;
    page_size = sysconf(_SC_PAGE_SIZE);
    perf_session* session = perf_session__new(&file, false, nullptr);
    if (!session)
        throw std::runtime_error("failed to open file");

    process_events(session, [this](perf_event *event,
                   perf_sample *sample,
                   perf_evsel *evsel, machine *machine) -> int
       {
           const char *evname = perf_evsel__name(evsel);
           if (evsel->attr.type != PERF_TYPE_HARDWARE || evsel->attr.config != PERF_COUNT_HW_CPU_CYCLES)
           {
               std::cout << "ignoring " << evname << std::endl;
               return 0;
           }

           addr_location al;
           if (perf_event__preprocess_sample(event, machine, &al, sample) < 0) {
               std::cout << "problem processing " << event->header.type << " event, skipping it.\n";
               return -1;
           }

           if (sample->callchain) {
               struct addr_location node_al;

               if (machine__resolve_callchain(al.machine, evsel, al.thread,
                                  sample, NULL, NULL,
                                  PERF_MAX_STACK_DEPTH) != 0) {
                   std::cout << "error resolving callchain\n";
                   return 0;
               }
               callchain_cursor_commit(&callchain_cursor);

               std::vector<string_ref> funcs;

               for (size_t stack_depth = 0; stack_depth != 100; ++stack_depth)
               {
                   u64 addr = 0;

                   callchain_cursor_node *node = callchain_cursor_current(&callchain_cursor);
                   if (!node)
                       break;

                   if (!node->sym || !node->sym->ignore)
                   {
                       std::stringstream ss;
                       ss << get_sym_name(node->sym) << " (" << get_dso_name(node->map) << ")";
                       funcs.push_back(frame_names.put(ss.str()));
                   }

                   callchain_cursor_advance(&callchain_cursor);
               }

               samples.push_back(backtrace(std::move(funcs)));
           }
           else
           {
               /*char const* sym = "[unknown]";
               if (al.sym)
                   sym = al.sym->name;
               char const* dso = "[unknown]";
               if (al.map->dso)
                   dso = al.map->dso->long_name;
               std::cout << std::hex << sample->ip << " " << sym << " " << dso << std::endl;*/
           }
           return 0;
       });

    perf_session__delete(session);
}
