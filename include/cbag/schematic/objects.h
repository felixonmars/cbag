//
// Created by erichang on 6/27/18.
//

#ifndef CBAG_SCHEMATIC_OBJECTS_H
#define CBAG_SCHEMATIC_OBJECTS_H

#include <string>
#include <tuple>
#include <utility>

#include <cbag/common.h>

namespace cbag {

    /** An instance in a schematic.
     */
    struct CSchInstance {
        /** Create an empty instance.
         */
        CSchInstance() = default;

        /** Create an instance with empty parameter and terminal mappings.
         *
         * @param name name of the instance.
         * @param lib the library name.
         * @param cell the cell name.
         * @param view the view name.
         * @param xform the instance location.
         */
        CSchInstance(NameUnit name, std::string lib, std::string cell, std::string view, Transform xform)
                : inst_name(std::move(name)), lib_name(std::move(lib)),
                  cell_name(std::move(cell)), view_name(std::move(view)),
                  xform(xform) {}

        // This class is movable, but not copyable.
        // This ensures good performance when placed in STL containers.

        CSchInstance(const CSchInstance &) = delete;

        CSchInstance &operator=(const CSchInstance &) = delete;

        CSchInstance(CSchInstance &&) noexcept;

        CSchInstance &operator=(CSchInstance &&) noexcept;

        /** Returns true if this instance is a vector instance.
         *
         *  @returns true if this instance is a vector instance.
         */
        inline bool is_vector() { return inst_name.is_vector(); }

        /** Returns the number of instances this schematic instance represents.
         *
         *  @returns number of instances in this schematic instance.
         */
        inline uint32_t size() { return (inst_name.size()); }

        inline bool operator<(const CSchInstance &other) const { return inst_name < other.inst_name; }

        NameUnit inst_name;
        std::string lib_name, cell_name, view_name;
        Transform xform;
        std::vector<Name> in_pins, out_pins, io_pins;
        std::map<NameUnit, NameUnit> connections;
        ParamMap params;
    };

    inline CSchInstance::CSchInstance(CSchInstance &&) noexcept = default;

    inline CSchInstance &CSchInstance::operator=(CSchInstance &&) noexcept = default;


    /** A schematic master, in other words, a schematic cellview.
     *
     *  Note that the terminal lists and instance list are all sorted in ascending order.
     */
    struct CSchMaster {
        CSchMaster() = default;

        // This class is movable, but not copyable.
        // This ensures good performance when placed in STL containers.

        CSchMaster(const CSchMaster &) = delete;

        CSchMaster &operator=(const CSchMaster &) = delete;

        CSchMaster(CSchMaster &&) noexcept;

        CSchMaster &operator=(CSchMaster &&) noexcept;

        std::vector<Name> in_pins, out_pins, io_pins;
        std::list<CSchInstance> inst_list;
    };

    inline CSchMaster::CSchMaster(CSchMaster &&) noexcept = default;

    inline CSchMaster &CSchMaster::operator=(CSchMaster &&) noexcept = default;


    // YAML stream out functions.

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchInstance &v);

    YAML::Emitter &operator<<(YAML::Emitter &out, const CSchMaster &v);

}

#endif //CBAG_SCHEMATIC_OBJECTS_H
