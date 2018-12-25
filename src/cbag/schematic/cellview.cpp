
#include <fmt/core.h>

#include <cbag/spirit/name.h>
#include <cbag/spirit/name_rep.h>
#include <cbag/spirit/parsers.h>

#include <cbag/common/box_t_util.h>
#include <cbag/common/param_map_util.h>
#include <cbag/schematic/cellview.h>
#include <cbag/schematic/cellview_info.h>
#include <cbag/schematic/instance.h>
#include <cbag/util/io.h>
#include <cbag/yaml/cellviews.h>

namespace cbag {
namespace sch {

cellview::cellview() = default;

cellview::cellview(const std::string &yaml_fname, const std::string &sym_view) {
    if (!util::is_file(yaml_fname)) {
        throw std::invalid_argument(yaml_fname + " is not a file.");
    }

    auto n = YAML::LoadFile(yaml_fname);

    *this = n.as<cellview>();

    if (!sym_view.empty()) {
        // load symbol cellview
        auto yaml_path = cbag::util::get_canonical_path(yaml_fname);
        yaml_path.replace_extension(fmt::format(".{}{}", sym_view, yaml_path.extension().c_str()));
        if (cbag::util::is_file(yaml_path)) {
            auto s = YAML::LoadFile(yaml_path.string());
            sym_ptr = std::make_unique<cellview>(s.as<cellview>());
        }
    }
}

cellview::cellview(std::string lib_name, std::string cell_name, std::string view_name, coord_t xl,
                   coord_t yl, coord_t xh, coord_t yh)
    : lib_name(std::move(lib_name)), cell_name(std::move(cell_name)),
      view_name(std::move(view_name)), bbox(xl, yl, xh, yh) {}

cellview_info cellview::get_info(const std::string &name) const {
    cellview_info ans(name, false);

    for (auto const &pair : terminals) {
        switch (pair.second.ttype) {
        case term_type::input:
            ans.in_terms.push_back(pair.first);
            break;
        case term_type::output:
            ans.out_terms.push_back(pair.first);
            break;
        default:
            ans.io_terms.push_back(pair.first);
        }
    }

    return ans;
}

void cellview::to_file(const std::string &fname) const {
    // create root directory if not exist.
    auto outfile = util::open_file_write(fname);
    YAML::Node node(*this);
    YAML::Emitter emitter;
    emitter << node;
    outfile << emitter.c_str() << std::endl;
    outfile.close();
}

std::unique_ptr<cellview> cellview::get_copy() const {
    // copy easily copiable attributes
    cellview ans(lib_name, cell_name, view_name, xl(bbox), yl(bbox), xh(bbox), yh(bbox));
    ans.terminals = terminals;
    ans.shapes = shapes;
    ans.props = props;
    ans.app_defs = app_defs;

    // copy instances
    for (const auto &pair : instances) {
        ans.instances.emplace_back(pair.first, pair.second->get_copy());
    }
    // copy symbol pointer
    if (sym_ptr != nullptr) {
        ans.sym_ptr = sym_ptr->get_copy();
    }

    return std::make_unique<cellview>(std::move(ans));
}

void cellview::clear_params() { props.clear(); }

void cellview::set_param(std::string name, const param_t &val) {
    cbag::set_param(props, std::move(name), val);
}

void cellview::rename_pin(const std::string &old_name, const std::string &new_name) {
    // check the new pin name does not exist already
    if (terminals.find(new_name) != terminals.end()) {
        throw std::invalid_argument(fmt::format("Terminal {} already exists.", new_name));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, spirit::name(), ast);

    if (terminals.replace_key(old_name, new_name) == terminals.end()) {
        throw std::invalid_argument("Cannot find terminal: " + old_name);
    }

    // rename the corresponding symbol pin
    if (sym_ptr != nullptr) {
        sym_ptr->rename_pin(old_name, new_name);
    }
}

void cellview::add_pin(const std::string &new_name, enum_t term_type) {
    // check the pin name is legal.  Parse will throw exception if not passed
    spirit::ast::name ast;
    parse(new_name, spirit::name(), ast);

    // check the pin name does not exist already
    if (terminals.find(new_name) != terminals.end()) {
        throw std::invalid_argument(fmt::format("Terminal {} already exists.", new_name));
    }
    /*
    // get the map to insert
    cbag::util::sorted_map<std::string, PinFigure> *ptr = nullptr;
    switch (term_type) {
    case trmInput:
        ptr = &in_terms;
        break;
    case trmOutput:
        ptr = &out_terms;
        break;
    case trmInout:
        ptr = &io_terms;
        break;
    default:
        throw std::invalid_argument(fmt::format("Bad terminal type: {}", term_type));
    }

    // insert into map
    ptr->emplace(std::move(key), PinFigure(Rect(0, 0, "", 0, 0, 10, 10), stSignal));
    */

    // TODO: add implementation
    throw std::runtime_error("add_pin functionality not implemented yet.  See developer.");
}

bool cellview::remove_pin(const std::string &name) {
    auto success = terminals.erase(name) > 0;
    // remove symbol pin
    if (success && sym_ptr != nullptr) {
        sym_ptr->remove_pin(name);
    }
    return success;
}

void cellview::rename_instance(const std::string &old_name, std::string new_name) {
    // check the new name does not exist
    if (instances.find(new_name) != instances.end()) {
        throw std::invalid_argument(fmt::format("instance {} already exists.", new_name));
    }
    // check the new name is legal.  Parse will throw exception if not passed
    spirit::ast::name_rep new_ast;
    parse(new_name, spirit::name_rep(), new_ast);

    auto iter = instances.replace_key(old_name, new_name);
    if (iter == instances.end())
        throw std::invalid_argument("Cannot find instance: " + old_name);

    // resize nets if necessary
    spirit::ast::name_rep old_ast;
    parse(old_name, spirit::name_rep(), old_ast);
    auto old_size = old_ast.size();
    auto new_size = new_ast.size();
    if (old_size != new_size) {
        iter->second->resize_nets(old_size, new_size);
    }
}

bool cellview::remove_instance(const std::string &name) { return instances.erase(name) > 0; }

} // namespace sch
} // namespace cbag
