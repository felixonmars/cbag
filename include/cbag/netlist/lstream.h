
#ifndef CBAG_NETLIST_LSTREAM_H
#define CBAG_NETLIST_LSTREAM_H

#include <fstream>
#include <string>
#include <vector>

#include <cbag/spirit/namespace_info.h>

namespace cbag {
namespace netlist {
/** A class that formats a line in the netlist
 */

class lstream {
  private:
    std::vector<std::string> tokens;
    struct helper;

  public:
    class back_inserter {
      private:
        lstream *stream_ = nullptr;

      public:
        explicit back_inserter(lstream *stream);
        back_inserter &operator*();
        back_inserter &operator=(std::string name);
    };

    lstream();

    bool empty() const;

    back_inserter get_back_inserter();

    lstream &append_last(const char *seq);

    lstream &append_last(const std::string &seq);

    lstream &operator<<(const std::string &token);

    lstream &operator<<(std::string &&token);

    lstream &operator<<(const std::vector<std::string> &tokens);

    lstream &operator<<(std::vector<std::string> &&tokens);

    std::ofstream &to_file(std::ofstream &stream, spirit::namespace_cdba,
                           bool newline = true) const;

    std::ofstream &to_file(std::ofstream &stream, spirit::namespace_cdl_cmd,
                           bool newline = true) const;

    std::ofstream &to_file(std::ofstream &stream, spirit::namespace_verilog,
                           bool newline = true) const;
};

} // namespace netlist
} // namespace cbag

#endif
