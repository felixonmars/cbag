/** \file database.cpp
 *  \brief This file implements classes and functions to read/write an OpenAccess database.
 *
 *  \author Eric Chang
 *  \date   2018/07/10
 */

// TODO: Debug only, remve later
#include <iostream>

#include <fmt/format.h>
// include fmt/ostream.h to support formatting oaStrings, which defines operator <<
#include <fmt/ostream.h>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>

#include <cbagoa/database.h>


namespace bsp = cbag::spirit;
namespace bsa = bsp::ast;

namespace cbagoa {
    bsa::name parse_name(const oa::oaString &source) {
        return cbag::parse<bsa::name, bsp::parser::name_type>(source, source.getLength(),
                                                              bsp::name());
    }

    bsa::name_unit parse_name_unit(const oa::oaString &source) {
        return cbag::parse<bsa::name_unit,
                bsp::parser::name_unit_type>(source, source.getLength(), bsp::name_unit());
    }

    cbag::Orientation convert_orient(const oa::oaOrient &orient) {
        switch (orient) {
            case oa::oacR0:
                return cbag::Orientation::R0;
            case oa::oacR90:
                return cbag::Orientation::R90;
            case oa::oacR180:
                return cbag::Orientation::R180;
            case oa::oacR270:
                return cbag::Orientation::R270;
            case oa::oacMY:
                return cbag::Orientation::MY;
            case oa::oacMYR90:
                return cbag::Orientation::MYR90;
            case oa::oacMX:
                return cbag::Orientation::MX;
            case oa::oacMXR90:
                return cbag::Orientation::MXR90;
            default:
                throw std::invalid_argument("Unknown orientation code.");
        }
    }

    cbag::PathStyle convert_path_style(const oa::oaPathStyle &style) {
        switch (style) {
            case oa::oacTruncatePathStyle:
                return cbag::PathStyle::truncate;
            case oa::oacExtendPathStyle:
                return cbag::PathStyle::extend;
            case oa::oacRoundPathStyle:
                return cbag::PathStyle::round;
            case oa::oacVariablePathStyle:
                return cbag::PathStyle::variable;
            default:
                throw std::invalid_argument("Unknown path style code.");
        }
    }

    cbag::PathStyle convert_end_style(const oa::oaEndStyle &style) {
        switch (style) {
            case oa::oacTruncateEndStyle:
                return cbag::PathStyle::truncate;
            case oa::oacExtendEndStyle:
                return cbag::PathStyle::extend;
            case oa::oacVariableEndStyle:
                return cbag::PathStyle::variable;
            case oa::oacChamferEndStyle:
                return cbag::PathStyle::chamfer;
            case oa::oacCustomEndStyle:
                return cbag::PathStyle::custom;
            default:
                throw std::invalid_argument("Unknown end style code.");
        }
    }

    cbag::Alignment convert_alignment(const oa::oaTextAlign &align) {
        switch (align) {
            case oa::oacUpperLeftTextAlign:
                return cbag::Alignment::upperLeft;
            case oa::oacCenterLeftTextAlign:
                return cbag::Alignment::centerLeft;
            case oa::oacLowerLeftTextAlign:
                return cbag::Alignment::lowerLeft;
            case oa::oacUpperRightTextAlign:
                return cbag::Alignment::upperRight;
            case oa::oacCenterRightTextAlign:
                return cbag::Alignment::centerRight;
            case oa::oacLowerRightTextAlign:
                return cbag::Alignment::lowerRight;
            case oa::oacUpperCenterTextAlign:
                return cbag::Alignment::upperCenter;
            case oa::oacCenterCenterTextAlign:
                return cbag::Alignment::centerCenter;
            case oa::oacLowerCenterTextAlign:
                return cbag::Alignment::lowerCenter;
            default:
                throw std::invalid_argument("Unknown text alignment code.");
        }
    }

    cbag::Font convert_font(const oa::oaFont &font) {
        switch (font) {
            case oa::oacEuroStyleFont:
                return cbag::Font::euroStyle;
            case oa::oacGothicFont:
                return cbag::Font::gothic;
            case oa::oacMathFont:
                return cbag::Font::math;
            case oa::oacRomanFont:
                return cbag::Font::roman;
            case oa::oacScriptFont:
                return cbag::Font::script;
            case oa::oacStickFont:
                return cbag::Font::stick;
            case oa::oacFixedFont:
                return cbag::Font::fixed;
            case oa::oacSwedishFont:
                return cbag::Font::swedish;
            case oa::oacMilSpecFont:
                return cbag::Font::milSpec;
            default:
                throw std::invalid_argument("Unknown font code.");
        }
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        throw std::runtime_error("OA Error: " + std::string(msg));
    }

#pragma clang diagnostic pop

    OALibrary::~OALibrary() {
        close();
    }

    void OALibrary::open_lib(const std::string &lib_file, const std::string &library,
                             const std::string &lib_path, const std::string &tech_lib) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                     oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        // open library definition
        oa::oaString lib_def_file(lib_file.c_str());
        oa::oaLibDefList::openLibs(lib_def_file);

        // open library
        lib_name = library;
        lib_name_oa = oa::oaScalarName(ns, library.c_str());
        lib_ptr = oa::oaLib::find(lib_name_oa);
        if (lib_ptr == nullptr) {
            // create new library
            oa::oaString oa_lib_path(lib_path.c_str());
            oa::oaScalarName oa_tech_lib(ns, tech_lib.c_str());
            lib_ptr = oa::oaLib::create(lib_name_oa, oa_lib_path);
            oa::oaTech::attach(lib_ptr, oa_tech_lib);

            // NOTE: I cannot get open access to modify the library file, so
            // we just do it by hand.
            std::ofstream outfile;
            outfile.open(lib_file, std::ios_base::app);
            outfile << "DEFINE " << library << " " << lib_path << std::endl;
            outfile.close();
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library: " + library);
        }

        // open technology file
        tech_ptr = oa::oaTech::find(lib_ptr);
        if (tech_ptr == nullptr) {
            // opened tech not found, attempt to open
            if (!oa::oaTech::exists(lib_ptr)) {
                throw std::runtime_error("Cannot find technology for library: " + library);
            } else {
                tech_ptr = oa::oaTech::open(lib_ptr, 'r');
                if (tech_ptr == nullptr) {
                    throw std::runtime_error("Cannot open technology for library: " + library);
                }
            }
        }

        // get database unit
        dbu_per_uu = tech_ptr->getDBUPerUU(oa::oaViewType::get(oa::oacMaskLayout));

        is_open = true;
    }

    std::pair<oa::oaDesign *, oa::oaBlock *>
    OALibrary::open_design(const std::string &cell_name, const std::string &view_name) {
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name_oa, cell_oa, view_oa, 'r');
        if (dsn_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open cell: {}__{}({})",
                                                    lib_name, cell_name, view_name));
        }

        oa::oaBlock *blk_ptr = dsn_ptr->getTopBlock();
        if (blk_ptr == nullptr) {
            throw std::invalid_argument(fmt::format("Cannot open top block for cell: {}__{}({})",
                                                    lib_name, cell_name, view_name));
        }
        return {dsn_ptr, blk_ptr};
    }

    cbag::SchMaster OALibrary::parse_schematic(const std::string &cell_name,
                                               const std::string &view_name) {
        // get OA design and block pointers
        auto dsn_ptr_pair = open_design(cell_name, view_name);
        oa::oaDesign *dsn_ptr = dsn_ptr_pair.first;
        oa::oaBlock *blk_ptr = dsn_ptr_pair.second;

        // place holder classes
        oa::oaString tmp_str;
        // create schematic master
        cbag::SchMaster ans;

        // get pins
        oa::oaIter<oa::oaPin> pin_iter(blk_ptr->getPins());
        oa::oaPin *pin_ptr;
        while ((pin_ptr = pin_iter.getNext()) != nullptr) {
            oa::oaTerm *term_ptr = pin_ptr->getTerm();
            oa::oaString tmp_;
            term_ptr->getName(ns_cdba, tmp_str);
            bool success;
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    success = ans.in_pins.insert(parse_name(tmp_str)).second;
                    break;
                case oa::oacOutputTermType :
                    success = ans.out_pins.insert(parse_name(tmp_str)).second;
                    break;
                case oa::oacInputOutputTermType :
                    success = ans.io_pins.insert(parse_name(tmp_str)).second;
                    break;
                default:
                    term_ptr->getName(ns_cdba, tmp_str);
                    throw std::invalid_argument(fmt::format("Pin {} has invalid terminal type: {}",
                                                            tmp_str,
                                                            term_ptr->getTermType().getName()));
            }
            if (!success) {
                throw std::invalid_argument(
                        fmt::format("Cannot add pin {}; it already exists.", tmp_str));
            }
        }

        // get instances
        oa::oaIter<oa::oaInst> inst_iter(blk_ptr->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString inst_lib_oa, inst_cell_oa, inst_name_oa;
            inst_ptr->getLibName(ns_cdba, inst_lib_oa);
            inst_ptr->getCellName(ns_cdba, inst_cell_oa);
            // NOTE: exclude pin instances
            if (inst_lib_oa != "basic" ||
                (inst_cell_oa != "ipin" && inst_cell_oa != "opin" && inst_cell_oa != "iopin")) {
                // get view
                oa::oaString inst_view_oa;
                inst_ptr->getViewName(ns_cdba, inst_view_oa);

                // get instance name
                inst_ptr->getName(ns_cdba, inst_name_oa);

                // get transform
                oa::oaTransform xform;
                inst_ptr->getTransform(xform);

                // create schematic instance
                bsa::name_unit inst_name = parse_name_unit(inst_name_oa);
                if (inst_name.mult > 1) {
                    throw std::invalid_argument(
                            fmt::format("Invalid instance name: {}", inst_name_oa));
                }
                uint32_t inst_size = inst_name.size();

                cbag::SchInstance inst(std::string(inst_lib_oa), std::string(inst_cell_oa),
                                       std::string(inst_view_oa),
                                       cbag::Transform(xform.xOffset(), xform.yOffset(),
                                                       convert_orient(xform.orient())));
                auto inst_ret_val = ans.inst_map.emplace(std::move(inst_name), std::move(inst));
                if (!inst_ret_val.second) {
                    throw std::invalid_argument(
                            fmt::format("Instance {} already exists.", inst_name_oa));
                }

                // parse schematic instance attributes
                parse_sch_inst(&inst_ret_val.first->second, inst_ptr, inst_size);
            }
        }

        // close design and return master
        dsn_ptr->close();

        return ans;
    }

    void OALibrary::parse_sch_inst(cbag::SchInstance *sinst_ptr, oa::oaInst *inst_ptr,
                                   uint32_t inst_size) {
        // get parameters
        if (inst_ptr->hasProp()) {
            oa::oaIter<oa::oaProp> prop_iter(inst_ptr->getProps());
            oa::oaProp *prop_ptr;
            while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                add_param(sinst_ptr->params, prop_ptr);
            }
        }

        // get instance connections
        oa::oaIter<oa::oaInstTerm> iterm_iter(inst_ptr->getInstTerms(oacInstTermIterAll));
        oa::oaInstTerm *iterm_ptr;
        oa::oaString term_name_oa, net_name_oa;
        while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
            // get terminal and net names
            iterm_ptr->getTerm()->getName(ns_cdba, term_name_oa);
            iterm_ptr->getNet()->getName(ns_cdba, net_name_oa);
            bsa::name term_name = parse_name(term_name_oa);
            bsa::name net_name = parse_name(net_name_oa);

            // populate connection map
            auto tname_iter = term_name.begin();
            auto tname_end = term_name.end();
            auto nname_iter = net_name.begin();
            auto nname_end = net_name.end();
            if (inst_size == 1) {
                // handle case where we have a scalar instance
                for (; tname_iter != tname_end; ++tname_iter, ++nname_iter) {
                    if (nname_iter == nname_end) {
                        throw std::invalid_argument(
                                fmt::format("Instance terminal {} net {} length mismatch.",
                                            term_name_oa, net_name_oa));
                    }
                    bsa::name_bit term_name_bit = *tname_iter;

                    auto conn_ret = sinst_ptr->connections.emplace(*tname_iter, inst_size);
                    if (conn_ret.second) {
                        (conn_ret.first->second)[0] = *nname_iter;
                    } else {
                        throw std::invalid_argument(fmt::format("Instance has duplicate pin {}",
                                                                term_name_bit.to_string()));
                    }
                }
                if (nname_iter != nname_end) {
                    throw std::invalid_argument(
                            fmt::format("Instance terminal {} net {} length mismatch.",
                                        term_name_oa, net_name_oa));
                }
            } else {
                // handle case where we have a vector instance
                std::vector<std::map<bsa::name_bit,
                        std::vector<bsa::name_bit> >::iterator> ptr_list;
                for (; tname_iter != tname_end; ++tname_iter, ++nname_iter) {
                    if (nname_iter == nname_end) {
                        throw std::invalid_argument(
                                fmt::format("Instance terminal {} net {} length mismatch.",
                                            term_name_oa, net_name_oa));
                    }
                    bsa::name_bit term_name_bit = *tname_iter;

                    auto conn_ret = sinst_ptr->connections.emplace(*tname_iter, inst_size);
                    if (conn_ret.second) {
                        (conn_ret.first->second)[0] = *nname_iter;
                        ptr_list.push_back(conn_ret.first);
                    } else {
                        throw std::invalid_argument(fmt::format("Instance has duplicate pin {}",
                                                                term_name_bit.to_string()));
                    }
                }
                for (uint32_t idx = 1; idx < inst_size; ++idx) {
                    for (auto ptr : ptr_list) {
                        if (nname_iter == nname_end) {
                            throw std::invalid_argument(
                                    fmt::format("Instance {} terminal {} net {} length mismatch.",
                                                term_name_oa, net_name_oa));
                        }
                        (ptr->second)[idx] = *nname_iter;
                        ++nname_iter;
                    }
                }
            }
        }
    }

    cbag::SchSymbol
    OALibrary::parse_symbol(const std::string &cell_name, const std::string &view_name) {

        // return value
        cbag::SchSymbol ans;
        // OA design and block pointers
        auto dsn_ptr_pair = open_design(cell_name, view_name);
        oa::oaDesign *dsn_ptr = dsn_ptr_pair.first;
        oa::oaBlock *blk_ptr = dsn_ptr_pair.second;
        // temporary string variable
        oa::oaString tmp_str;

        // get pins
        oa::oaIter<oa::oaPin> pin_iter(blk_ptr->getPins());
        oa::oaPin *pin_ptr;
        while ((pin_ptr = pin_iter.getNext()) != nullptr) {
            // get terminal
            oa::oaTerm *term_ptr = pin_ptr->getTerm();
            term_ptr->getName(ns_cdba, tmp_str);
            bsa::name term_name = parse_name(tmp_str);
            // get terminal shapes
            oa::oaIter<oa::oaPinFig> fig_iter(pin_ptr->getFigs());
            oa::oaPinFig *fig_ptr = fig_iter.getNext();
            // check we have exactly one pin figure
            if (fig_ptr == nullptr) {
                throw std::invalid_argument(fmt::format("Pin {} has no shapes.", tmp_str));
            }
            if (fig_iter.getNext() != nullptr) {
                throw std::invalid_argument(fmt::format("Pin {} has more than one shape.",
                                                        tmp_str));
            }
            if (fig_ptr->getType() != oa::oacRectType) {
                throw std::invalid_argument(fmt::format("Pin {} shape is not rectangle", tmp_str));
            }
            oa::oaRect *pin_rect = static_cast<oa::oaRect *>(fig_ptr); // NOLINT
            oa::oaBox bbox;
            pin_rect->getBBox(bbox);
            oa::oaLayerNum ln = pin_rect->getLayerNum();
            oa::oaPurposeNum pn = pin_rect->getPurposeNum();
            oa::oaCoord xl = bbox.left();
            oa::oaCoord xh = bbox.right();
            oa::oaCoord yl = bbox.bottom();
            oa::oaCoord yh = bbox.top();

            // add pin
            bool success;
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    success = ans.in_pins.emplace(parse_name(tmp_str),
                                                  cbag::Rect(ln, pn, xl, yl, xh, yh)).second;
                    break;
                case oa::oacOutputTermType :
                    success = ans.out_pins.emplace(parse_name(tmp_str),
                                                   cbag::Rect(ln, pn, xl, yl, xh, yh)).second;
                    break;
                case oa::oacInputOutputTermType :
                    success = ans.io_pins.emplace(parse_name(tmp_str),
                                                  cbag::Rect(ln, pn, xl, yl, xh, yh)).second;
                    break;
                default:
                    throw std::invalid_argument(fmt::format("Pin {} has invalid terminal type: {}",
                                                            tmp_str,
                                                            term_ptr->getTermType().getName()));
            }
            if (!success) {
                throw std::invalid_argument(fmt::format("Cannot add pin {}; it already exists.",
                                                        tmp_str));
            }
        }  // end of pin loop

        // get all shapes
        oa::oaIter<oa::oaShape> shape_iter(blk_ptr->getShapes());
        oa::oaShape *shape_ptr;
        while ((shape_ptr = shape_iter.getNext()) != nullptr) {
            // skip shapes associated with pins.  We make those separately.
            if (!shape_ptr->hasPin()) {
                add_shape(ans, shape_ptr);
            }
        }

        // get Properties
        if (dsn_ptr->hasProp()) {
            oa::oaIter<oa::oaProp> prop_iter(dsn_ptr->getProps());
            oa::oaProp *prop_ptr;
            while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                add_param(ans.params, prop_ptr);
            }
        }

        // close design and return master
        dsn_ptr->close();

        return ans;
    }

    void OALibrary::close() {
        if (is_open) {
            tech_ptr->close();
            lib_ptr->close();

            is_open = false;
        }

    }

    cbag::Rect make_rect(oa::oaRect *p) {
        oa::oaBox box;
        p->getBBox(box);
        return {p->getLayerNum(), p->getPurposeNum(), box.left(), box.bottom(),
                box.right(), box.top()};
    }

    cbag::Poly make_poly(oa::oaPolygon *p) {
        oa::oaPointArray points;
        p->getPoints(points);
        size_t n = points.getNumElements();
        cbag::Poly ans(p->getLayerNum(), p->getPurposeNum(), n);
        for (size_t i = 0; i < n; ++i) {
            ans.points[i] = {points[i].x(), points[i].y()};
        }
        return ans;
    }

    cbag::Arc make_arc(oa::oaArc *p) {
        oa::oaBox box;
        p->getEllipseBBox(box);
        return {p->getLayerNum(), p->getPurposeNum(), p->getStartAngle(), p->getStopAngle(),
                box.left(), box.bottom(), box.right(), box.top()};
    }

    cbag::Donut make_donut(oa::oaDonut *p) {
        oa::oaPoint center;
        p->getCenter(center);
        return {p->getLayerNum(), p->getPurposeNum(), center.x(), center.y(),
                p->getRadius(), p->getHoleRadius()};
    }

    cbag::Ellipse make_ellipse(oa::oaEllipse *p) {
        oa::oaBox box;
        p->getBBox(box);
        return {p->getLayerNum(), p->getPurposeNum(), box.left(), box.bottom(),
                box.right(), box.top()};
    }

    cbag::Line make_line(oa::oaLine *p) {
        oa::oaPointArray points;
        p->getPoints(points);
        size_t n = points.getNumElements();
        cbag::Line ans(p->getLayerNum(), p->getPurposeNum(), n);
        for (size_t i = 0; i < n; ++i) {
            ans.points[i] = {points[i].x(), points[i].y()};
        }
        return ans;
    }

    cbag::Path make_path(oa::oaPath *p) {
        oa::oaPointArray points;
        p->getPoints(points);
        size_t n = points.getNumElements();
        cbag::Path ans(p->getLayerNum(), p->getPurposeNum(), p->getWidth(), n,
                       convert_path_style(p->getStyle()), p->getBeginExt(), p->getEndExt());
        for (size_t i = 0; i < n; ++i) {
            ans.points[i] = {points[i].x(), points[i].y()};
        }
        return ans;
    }

    cbag::PathSeg make_path_seg(oa::oaPathSeg *p) {
        oa::oaSegStyle style;
        oa::oaPoint p0, p1;
        p->getStyle(style);
        p->getPoints(p0, p1);
        oa::oaDist e0, e1, ld0, ld1, rd0, rd1, rh0, rh1;
        style.getBeginExt(e0, ld0, rd0, rh0);
        style.getEndExt(e1, ld1, rd1, rh1);
        cbag::SegEndStyle s0(convert_end_style(style.getBeginStyle()), e0, ld0, rd0, rh0);
        cbag::SegEndStyle s1(convert_end_style(style.getEndStyle()), e1, ld1, rd1, rh1);
        return {p->getLayerNum(), p->getPurposeNum(), p0.x(), p0.y(), p1.x(), p1.y(),
                style.getWidth(), s0, s1};
    }

    cbag::Text make_text(oa::oaText *p) {
        oa::oaPoint orig;
        oa::oaString text;
        p->getOrigin(orig);
        p->getText(text);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        return {p->getLayerNum(), p->getPurposeNum(), std::string(text), orig.x(), orig.y(),
                convert_alignment(p->getAlignment()), convert_orient(p->getOrient()),
                convert_font(p->getFont()), p->getHeight(), overbar, visible, drafting};
    }

    cbag::EvalText make_eval_text(oa::oaEvalText *p) {
        oa::oaPoint orig;
        oa::oaString text, eval;
        p->getOrigin(orig);
        p->getText(text);
        p->getEvaluatorName(eval);
        bool overbar = (p->hasOverbar() != 0);
        bool visible = (p->isVisible() != 0);
        bool drafting = (p->isDrafting() != 0);
        return {p->getLayerNum(), p->getPurposeNum(), std::string(text), orig.x(), orig.y(),
                convert_alignment(p->getAlignment()), convert_orient(p->getOrient()),
                convert_font(p->getFont()), p->getHeight(), overbar, visible,
                drafting, std::string(eval)};
    }

    void add_shape(cbag::SchSymbol &symbol, oa::oaShape *shape_ptr) {
        // get parameter value
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (shape_ptr->getType()) {
            case oa::oacRectType : {
                symbol.shapes.emplace_back(
                        make_rect(static_cast<oa::oaRect *>(shape_ptr))); // NOLINT
                break;
            }
            case oa::oacPolygonType : {
                symbol.shapes.emplace_back(
                        make_poly(static_cast<oa::oaPolygon *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacArcType : {
                symbol.shapes.emplace_back(
                        make_arc(static_cast<oa::oaArc *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacDonutType : {
                symbol.shapes.emplace_back(
                        make_donut(static_cast<oa::oaDonut *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacEllipseType : {
                symbol.shapes.emplace_back(
                        make_ellipse(static_cast<oa::oaEllipse *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacLineType : {
                symbol.shapes.emplace_back(
                        make_line(static_cast<oa::oaLine *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacPathType : {
                symbol.shapes.emplace_back(
                        make_path(static_cast<oa::oaPath *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacPathSegType : {
                symbol.shapes.emplace_back(
                        make_path_seg(static_cast<oa::oaPathSeg *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacTextType : {
                symbol.shapes.emplace_back(
                        make_text(static_cast<oa::oaText *>(shape_ptr)));  // NOLINT
                break;
            }
            case oa::oacEvalTextType : {
                symbol.shapes.emplace_back(
                        make_eval_text(static_cast<oa::oaEvalText *>(shape_ptr)));  // NOLINT
                break;
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA shape type: {}, see developer.",
                                    shape_ptr->getType().getName()));
            }
        }
    }

    void add_param(cbag::ParamMap &params, oa::oaProp *prop_ptr) {
        // get parameter name
        oa::oaString tmp_str;
        prop_ptr->getName(tmp_str);
        std::string key(tmp_str);

        // get parameter value
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        switch (prop_ptr->getType()) {
            case oa::oacStringPropType : {
                prop_ptr->getValue(tmp_str);
                params.emplace(key, std::string(tmp_str));
                break;
            }
            case oa::oacIntPropType : {
                params.emplace(key, static_cast<oa::oaIntProp *>(prop_ptr)->getValue()); // NOLINT
                break;
            }
            case oa::oacDoublePropType : {
                params.emplace(key,
                               static_cast<oa::oaDoubleProp *>(prop_ptr)->getValue()); // NOLINT
                break;
            }
            case oa::oacFloatPropType : {
                params.emplace(key, static_cast<double>(
                        static_cast<oa::oaFloatProp *>(prop_ptr)->getValue())); // NOLINT
                break;
            }
            case oa::oacTimePropType : {
                params.emplace(key, cbag::Time(
                        static_cast<oa::oaTimeProp *>(prop_ptr)->getValue())); // NOLINT
                break;
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                static_cast<oa::oaAppProp *>(prop_ptr)->getValue(data); // NOLINT
                params.emplace(key, cbag::Binary(data.getElements(), data.getNumElements()));
                break;
            }
            default : {
                throw std::invalid_argument(
                        fmt::format("Unsupported OA property {} with type: {}, see developer.",
                                    key, prop_ptr->getType().getName()));
            }
        }
    }

}