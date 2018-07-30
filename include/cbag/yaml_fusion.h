/** \file serialization.h
 *  \brief This file use yaml-cpp and Boost fusion to achieve easy
 * serialization.
 *
 *  The code is entirely copied from:
 *  http://jrruethe.github.io/blog/2015/08/17/yaml-de-slash-serialization-with-boost-fusion
 *
 *  \author Eric Chang
 *  \date   2018/07/17
 */

#ifndef CBAG_SERIALIZATION_H
#define CBAG_SERIALIZATION_H

#include <string>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/size.hpp>

// boost::fusion::extension::struct_member_name
#include <boost/fusion/adapted/struct/detail/extension.hpp>

// boost::fusion::result_of::value_at
#include <boost/fusion/include/value_at.hpp>
#include <boost/fusion/sequence/intrinsic/value_at.hpp>

// boost::fusion::result_of::size
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/sequence/intrinsic/size.hpp>

// BOOST_TYPEOF
#include <boost/typeof/typeof.hpp>

// boost::fusion::at
#include <boost/fusion/include/at.hpp>
#include <boost/fusion/sequence/intrinsic/at.hpp>

// boost::fusion::for_each
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

// boost::fusion::zip
#include <boost/fusion/algorithm/transformation/zip.hpp>
#include <boost/fusion/include/zip.hpp>

// boost::units::detail::demangle
#include <boost/units/detail/utility.hpp>

#include <yaml-cpp/yaml.h>

namespace yaml {
namespace serialization {
template <typename S> struct sequence {
    // Point to the first element
    typedef boost::mpl::int_<0> begin;

    // Point to the element after the last element in the sequence
    typedef typename boost::fusion::result_of::size<S>::type end;

    // Point to the first element
    typedef boost::mpl::int_<0> first;

    // Point to the second element (for pairs)
    typedef boost::mpl::int_<1> second;

    // Point to the last element in the sequence
    typedef typename boost::mpl::prior<end>::type last;

    // Number of elements in the sequence
    typedef typename boost::fusion::result_of::size<S>::type size;

    // Get a range representing the size of the structure
    typedef boost::mpl::range_c<unsigned int, 0, boost::mpl::size<S>::value>
        indices;
};

template <typename S, typename N> struct element_at {
    // Type of the element at this index
    typedef typename boost::fusion::result_of::value_at<S, N>::type type;

    // Previous element
    typedef typename boost::mpl::prior<N>::type previous;

    // Next element
    typedef typename boost::mpl::next<N>::type next;

    // Member name of the element at this index
    static inline std::string name() {
        return boost::fusion::extension::struct_member_name<S,
                                                            N::value>::call();
    }

    // Type name of the element at this index
    static inline std::string type_name() {
        return boost::units::detail::demangle(typeid(type).name());
    }

    // Access the element
    static inline typename boost::fusion::result_of::at<S const, N>::type
    get(S const &s) {
        return boost::fusion::at<N>(s);
    }
};

template <typename T> struct type {
    // Return the string name of the type
    static inline std::string name() {
        return boost::units::detail::demangle(typeid(T).name());
    }
};

template <typename T> struct inserter {
    typedef T Type;

    inserter(YAML::Node &subroot) : mSubroot(subroot) {}

    template <typename Zip> void operator()(Zip const &zip) const {
        typedef typename boost::remove_const<typename boost::remove_reference<
            typename boost::fusion::result_of::at_c<Zip, 0>::type>::type>::type
            Index;

        // Get the field name as a string using reflection
        std::string field_name = element_at<Type, Index>::name();

        // Get the field type
        typedef BOOST_TYPEOF(boost::fusion::at_c<1>(zip)) FieldType;

        // Alias the member
        FieldType const &member = boost::fusion::at_c<1>(zip);

        // Store this field in the yaml node
        mSubroot[field_name] = member;
    }

  protected:
    YAML::Node &mSubroot;
};

template <typename T> struct extractor {
    typedef T Type;

    extractor(YAML::Node &subroot) : mSubroot(subroot), mItem(0) {}

    template <typename Zip> void operator()(Zip const &zip) const {
        typedef typename boost::remove_const<typename boost::remove_reference<
            typename boost::fusion::result_of::at_c<Zip, 0>::type>::type>::type
            Index;

        // Get the field name as a string using reflection
        std::string field_name = element_at<Type, Index>::name();

        // Get the field native type
        typedef BOOST_TYPEOF(boost::fusion::at_c<1>(zip)) FieldType;

        // Alias the member
        // We need to const cast this because "boost::fusion::for_each"
        // requires that zip be const, however we want to modify it.
        FieldType const &const_member = boost::fusion::at_c<1>(zip);
        FieldType &member = const_cast<FieldType &>(const_member);

        // We need to const cast this because "boost::fusion::for_each"
        // requires that operator() be const, however we want to modify
        // the object. This item number is used for error reporting.
        int const &const_item = mItem;
        int &item = const_cast<int &>(const_item);

        // Try to load the value from the file
        try {
            // Extract this field from the yaml node
            member = mSubroot[field_name].as<FieldType>();

            // This item number helps us find issues when loading incomplete
            // yaml files
            ++item;
        }
        // Catch any exceptions
        catch (YAML::Exception const &e) {
            std::string type_name = type<FieldType>::name();

            throw std::invalid_argument("Error loading item.");
        }
    }

  protected:
    YAML::Node &mSubroot;
    int mItem;
};

template <typename Base> std::string to_yaml(const Base &v) {
    YAML::Emitter emitter;
    YAML::Node root = YAML::convert<Base>::encode(v);
    emitter << root;
    return emitter.c_str();
}

// Load yaml into this object
template <typename Base>
bool from_yaml(std::string const &yaml_string, Base &obj) {
    YAML::Node n;
    try {
        n = YAML::Load(yaml_string);
    } catch (...) {
        return false;
    }
    return YAML::convert<Base>::decode(n, obj);
}
} // namespace serialization
} // namespace yaml

namespace YAML {
template <typename T> struct convert {
    // This function will only be available if the template parameter is a boost
    // fusion sequence
    static Node encode(
        T const &rhs,
        typename boost::enable_if<
            typename boost::fusion::traits::is_sequence<T>::type>::type * = 0) {
        // For each item in T
        // Call inserter recursively
        // Every sequence is made up of primitives at some level

        // Get a range representing the size of the structure
        typedef typename yaml::serialization::sequence<T>::indices indices;

        // Make a root node to insert into
        YAML::Node root;

        // Create an inserter for the root node
        yaml::serialization::inserter<T> inserter(root);

        // Insert each member of the structure
        boost::fusion::for_each(boost::fusion::zip(indices(), rhs), inserter);

        return root;
    }

    // This function will only be available if the template parameter is a boost
    // fusion sequence
    static bool decode(
        Node const &node, T &rhs,
        typename boost::enable_if<
            typename boost::fusion::traits::is_sequence<T>::type>::type * = 0) {
        // For each item in T
        // Call extractor recursively
        // Every sequence is made up of primitives at some level

        // Get a range representing the size of the structure
        typedef typename yaml::serialization::sequence<T>::indices indices;

        // Create an extractor for the root node
        // Yaml-cpp requires node to be const&, but the extractor makes
        // non-const calls to it.
        Node &writable_node = const_cast<Node &>(node);
        yaml::serialization::extractor<T> extractor(writable_node);

        // Extract each member of the structure
        try {
            // An exception is thrown if any item in the loop cannot be read
            boost::fusion::for_each(boost::fusion::zip(indices(), rhs),
                                    extractor);
        }
        // Catch all exceptions and prevent them from propagating
        catch (...) {
            return false;
        }
        // If we made it here, all fields were read correctly
        return true;
    }
};
} // namespace YAML
#endif // CBAG_SERIALIZATION_H
