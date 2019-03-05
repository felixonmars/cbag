#include <tuple>

#include <cbag/util/binary_iterator.h>

#include <cbag/common/transformation_util.h>
#include <cbag/layout/cellview.h>
#include <cbag/layout/grid_object.h>
#include <cbag/layout/routing_grid_util.h>
#include <cbag/layout/tech_util.h>
#include <cbag/layout/via_util.h>
#include <cbag/layout/via_wrapper.h>

namespace cbag {
namespace layout {

class poly45_writer {
  public:
    using value_type = polygon45;

  private:
    geo_index &index;
    const routing_grid &grid;
    layer_t key;
    level_t lev;
    value_type last;
    bool has_value = false;

  public:
    poly45_writer(geo_index &index, const routing_grid &grid, layer_t key, level_t lev)
        : index(index), grid(grid), key(key), lev(lev) {}

    void push_back(value_type &&v) {
        record_last();
        last = std::move(v);
        has_value = true;
    }

    void insert(value_type *ptr, const value_type &v) {
        record_last();
        last = v;
        has_value = true;
    }

    void record_last() const {
        if (has_value) {
            auto[spx, spy] = get_margins(grid, key, lev, last);
            index.insert(last, spx, spy);
        }
    }

    value_type &back() { return last; }

    value_type *end() const { return nullptr; }
};

struct cellview::helper {
    static void add_inst(cellview &self, const instance &inst) {
        auto &inst_name = inst.get_inst_name();
        if (!inst_name.empty()) {
            // test if given name is valid
            if (self.inst_map.emplace(inst_name, inst).second)
                return;
        }
        auto map_end = self.inst_map.end();
        cbag::util::binary_iterator<cnt_t> iter(self.inst_name_cnt);
        while (iter.has_next()) {
            if (self.inst_map.find("X" + std::to_string(*iter)) == map_end) {
                iter.save();
                iter.down();
            } else {
                iter.up();
            }
        }

        self.inst_name_cnt = *(iter.get_save());
        self.inst_map.emplace("X" + std::to_string(self.inst_name_cnt), inst);
    }

    static geometry &make_geometry(cellview &self, layer_t key) {
        auto iter = self.geo_map.find(key);
        if (iter == self.geo_map.end()) {
            iter = self.geo_map.emplace(std::move(key), geometry(self.geo_mode)).first;
        }
        return iter->second;
    }

    static geo_index &get_geo_index(cellview &self, level_t lev) {
        return self.index_list[lev - self.get_grid()->get_bot_level()];
    }

    template <typename T> static void add_shape(cellview &self, layer_t key, const T &obj) {
        auto &geo = make_geometry(self, key);
        geo.add_shape(obj);

        auto lev_opt = self.get_tech()->get_level(key);
        if (lev_opt) {
            auto &grid = *self.get_grid();
            auto &index = get_geo_index(self, *lev_opt);
            if constexpr (std::is_base_of_v<polygon45_set, std::decay_t<T>>) {
                poly45_writer writer(index, grid, key, *lev_opt);
                obj.get(writer);
                writer.record_last();
            } else {
                auto[spx, spy] = get_margins(grid, key, *lev_opt, obj);
                index.insert(obj, spx, spy);
            }
        }
    }
};

cellview::cellview(std::shared_ptr<const routing_grid> grid, std::string cell_name,
                   geometry_mode geo_mode)
    : geo_mode(geo_mode), grid_ptr(std::move(grid)), cell_name(std::move(cell_name)),
      index_list(grid_ptr->get_num_levels()) {}

bool cellview::operator==(const cellview &rhs) const noexcept {
    return geo_mode == rhs.geo_mode && *grid_ptr == *(rhs.grid_ptr) && cell_name == rhs.cell_name &&
           geo_map == rhs.geo_map && inst_map == rhs.inst_map && pin_map == rhs.pin_map &&
           via_list == rhs.via_list && lay_block_map == rhs.lay_block_map &&
           area_block_list == rhs.area_block_list && boundary_list == rhs.boundary_list &&
           label_list == rhs.label_list;
}

void cellview::set_geometry_mode(geometry_mode new_mode) {
    if (!empty())
        throw std::runtime_error("Cannot change geometry mode of non-empty layout.");
    geo_mode = new_mode;
}

auto cellview::find_geometry(layer_t key) const -> decltype(geo_map.find(key)) {
    return geo_map.find(key);
}

const std::string &cellview::get_name() const noexcept { return cell_name; }
const tech *cellview::get_tech() const noexcept { return grid_ptr->get_tech(); }
const routing_grid *cellview::get_grid() const noexcept { return grid_ptr.get(); }

bool cellview::empty() const noexcept {
    return geo_map.empty() && inst_map.empty() && via_list.empty() && lay_block_map.empty() &&
           area_block_list.empty() && boundary_list.empty() && pin_map.empty();
}

const geo_index &cellview::get_geo_index(level_t lev) const {
    return index_list[lev - get_grid()->get_bot_level()];
}

auto cellview::begin_inst() const -> decltype(inst_map.cbegin()) { return inst_map.cbegin(); }
auto cellview::end_inst() const -> decltype(inst_map.cend()) { return inst_map.cend(); }
auto cellview::begin_geometry() const -> decltype(geo_map.cbegin()) { return geo_map.cbegin(); }
auto cellview::end_geometry() const -> decltype(geo_map.cend()) { return geo_map.cend(); }
auto cellview::begin_via() const -> decltype(via_list.cbegin()) { return via_list.cbegin(); }
auto cellview::end_via() const -> decltype(via_list.cend()) { return via_list.cend(); }
auto cellview::begin_lay_block() const -> decltype(lay_block_map.cbegin()) {
    return lay_block_map.cbegin();
}
auto cellview::end_lay_block() const -> decltype(lay_block_map.cend()) {
    return lay_block_map.cend();
}
auto cellview::begin_area_block() const -> decltype(area_block_list.cbegin()) {
    return area_block_list.cbegin();
}
auto cellview::end_area_block() const -> decltype(area_block_list.cend()) {
    return area_block_list.cend();
}
auto cellview::begin_boundary() const -> decltype(boundary_list.cbegin()) {
    return boundary_list.cbegin();
}
auto cellview::end_boundary() const -> decltype(boundary_list.cend()) {
    return boundary_list.cend();
}
auto cellview::begin_pin() const -> decltype(pin_map.cbegin()) { return pin_map.cbegin(); }
auto cellview::end_pin() const -> decltype(pin_map.cend()) { return pin_map.cend(); }
auto cellview::begin_label() const -> decltype(label_list.cbegin()) { return label_list.cbegin(); }
auto cellview::end_label() const -> decltype(label_list.cend()) { return label_list.cend(); }

void cellview::add_pin(lay_t lay_id, std::string &&net, std::string &&label, box_t &&bbox) {
    auto iter = pin_map.find(lay_id);
    if (iter == pin_map.end()) {
        iter = pin_map.emplace(lay_id, std::vector<pin>()).first;
    }
    iter->second.emplace_back(std::move(bbox), std::move(net), std::move(label));
}

void cellview::add_label(layer_t &&key, transformation &&xform, std::string &&label,
                         offset_t height) {
    label_list.emplace_back(std::move(key), std::move(xform), std::move(label), height);
}

void cellview::add_object(const blockage &obj) {
    if (obj.get_type() == blockage_type::placement) {
        // area blockage
        area_block_list.push_back(obj);
    } else {
        auto layer = obj.get_layer();
        auto iter = lay_block_map.find(layer);
        if (iter == lay_block_map.end()) {
            iter = lay_block_map.emplace(layer, std::vector<blockage>()).first;
        }
        iter->second.push_back(obj);
    }
}

void cellview::add_object(const boundary &obj) { boundary_list.push_back(obj); }

void cellview::add_object(boundary &&obj) { boundary_list.push_back(std::move(obj)); }

void cellview::add_object(const via_wrapper &obj) {
    via_list.push_back(obj.v);
    if (obj.add_layers) {
        auto[bot_key, unused, top_key] = get_tech()->get_via_layer_purpose(obj.v.get_via_id());
        (void)unused;
        auto bot_box = get_bot_box(obj.v);
        auto top_box = get_top_box(obj.v);
        add_shape(bot_key, bot_box);
        add_shape(top_key, top_box);
    }
}

void cellview::add_object(const instance &obj) {
    helper::add_inst(*this, obj);
    auto master = obj.get_cellview();
    if (master != nullptr) {
        // NOTE: all routing_grids are guaranteed to have the same levels.
        auto &grid = *get_grid();
        transformation xform_copy(obj.xform);
        auto tot_dx = obj.spx * obj.nx;
        auto tot_dy = obj.spy * obj.ny;
        for (auto cur_lev = grid.get_bot_level(); cur_lev <= grid.get_top_level(); ++cur_lev) {
            auto &parent_index = helper::get_geo_index(*this, cur_lev);
            auto &inst_index = master->get_geo_index(cur_lev);
            for (decltype(obj.nx) ix = 0; ix < obj.nx; ++ix, move_by(xform_copy, obj.spx, 0)) {
                for (decltype(obj.ny) iy = 0; iy < obj.ny; ++iy, move_by(xform_copy, 0, obj.spy)) {
                    parent_index.insert(&inst_index, xform_copy);
                }
                move_by(xform_copy, 0, -tot_dy);
            }
            move_by(xform_copy, -tot_dx, 0);
        }
    }
}

void cellview::add_shape(layer_t key, const box_t &obj) { helper::add_shape(*this, key, obj); }
void cellview::add_shape(layer_t key, const polygon90 &obj) { helper::add_shape(*this, key, obj); }
void cellview::add_shape(layer_t key, const polygon45 &obj) { helper::add_shape(*this, key, obj); }
void cellview::add_shape(layer_t key, const polygon &obj) { helper::add_shape(*this, key, obj); }
void cellview::add_shape(layer_t key, const polygon45_set &obj) {
    helper::add_shape(*this, key, obj);
}

void cellview::add_warr(const track_id &tid, std::array<offset_t, 2> coord) {
    auto &grid = *get_grid();
    auto lev = tid.get_level();
    auto &index = helper::get_geo_index(*this, lev);
    for (auto iter = begin_rect(grid, tid, coord), stop = end_rect(grid, tid, coord); iter != stop;
         ++iter) {
        auto[key, box] = *iter;
        auto &geo = helper::make_geometry(*this, key);
        geo.add_shape(box);

        auto[spx, spy] = get_margins(grid, key, lev, box);
        index.insert(box, spx, spy);
    }
}

} // namespace layout
} // namespace cbag
