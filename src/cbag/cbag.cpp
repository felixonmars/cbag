/** \file cbag.cpp
 *  \brief This implements the top level header file for cbag library.
 *
 *  \author Eric Chang
 *  \date   2018/07/18
 */

#include <cstring>
#include <fstream>

#include <yaml-cpp/yaml.h>
#include <easylogging++.h>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name_unit.h>
#include <cbag/spirit/name.h>
#include <cbag/database/yaml_cellviews.h>

INITIALIZE_EASYLOGGINGPP // NOLINT


namespace cbag {
    void init_logging() {}

    void to_file(const SchCellView &cv, const char *fname) {
        std::ofstream outfile(fname, std::ios_base::out);
        YAML::Node node(cv);
        YAML::Emitter emitter;
        emitter << node;
        outfile << emitter.c_str() << std::endl;
        outfile.close();
    }

    spirit::ast::name_unit parse_cdba_name_unit(const std::string &source) {
        spirit::ast::name_unit ast;
        parse(source.c_str(), source.size(), spirit::name_unit(), ast);
        return ast;
    }

    spirit::ast::name_unit parse_cdba_name_unit(const char *source) {
        spirit::ast::name_unit ast;
        parse(source, strlen(source), spirit::name_unit(), ast);
        return ast;
    }

    spirit::ast::name parse_cdba_name(const std::string &source) {
        spirit::ast::name ast;
        parse(source.c_str(), source.size(), spirit::name(), ast);
        return ast;
    }

    spirit::ast::name parse_cdba_name(const char *source) {
        spirit::ast::name ast;
        parse(source, strlen(source), spirit::name(), ast);
        return ast;
    }
}
