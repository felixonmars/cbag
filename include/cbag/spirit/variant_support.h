#ifndef CBAG_SPIRIT_VARIANT_SUPPORT_H
#define CBAG_SPIRIT_VARIANT_SUPPORT_H

// hack from https://github.com/boostorg/spirit/issues/270 to get
// X3 to work with std::variant
#include <variant>

#include <boost/mpl/vector.hpp>
#include <boost/spirit/home/x3/support/traits/is_variant.hpp>
#include <boost/spirit/home/x3/support/traits/tuple_traits.hpp>
#include <boost/spirit/home/x3/support/traits/variant_find_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/variant_has_substitute.hpp>

// Based on: boost/spirit/home/x3/support/traits/variant_find_substitute.hpp
namespace boost::spirit::x3::traits {
template <typename... Ts> struct is_variant<std::variant<Ts...>> : mpl::true_ {};

template <typename... Ts, typename Attribute>
struct variant_find_substitute<std::variant<Ts...>, Attribute> {
    typedef std::variant<Ts...> variant_type;
    typedef mpl::vector<Ts...> types;
    typedef typename mpl::end<types>::type end;

    typedef typename mpl::find_if<types, is_same<mpl::_1, Attribute>>::type iter_1;

    typedef typename mpl::eval_if<is_same<iter_1, end>,
                                  mpl::find_if<types, traits::is_substitute<mpl::_1, Attribute>>,
                                  mpl::identity<iter_1>>::type iter;

    typedef
        typename mpl::eval_if<is_same<iter, end>, mpl::identity<Attribute>, mpl::deref<iter>>::type
            type;
};

template <typename... Ts, typename Attribute>
struct variant_has_substitute_impl<std::variant<Ts...>, Attribute> {
    // Find a type from the variant that can be a substitute for Attribute.
    // return true_ if one is found, else false_

    typedef mpl::vector<Ts...> types;

    typedef typename mpl::end<types>::type end;

    typedef typename mpl::find_if<types, is_same<mpl::_1, Attribute>>::type iter_1;

    typedef typename mpl::eval_if<is_same<iter_1, end>,
                                  mpl::find_if<types, traits::is_substitute<mpl::_1, Attribute>>,
                                  mpl::identity<iter_1>>::type iter;

    typedef mpl::not_<is_same<iter, end>> type;
};
} // namespace boost::spirit::x3::traits

#endif
