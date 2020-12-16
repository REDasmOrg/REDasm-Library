// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/PEGTL/

#ifndef TAO_JSON_PEGTL_INTERNAL_DEPENDENT_FALSE_HPP
#define TAO_JSON_PEGTL_INTERNAL_DEPENDENT_FALSE_HPP

#include "../config.hpp"

namespace TAO_JSON_PEGTL_NAMESPACE::internal
{
   template< typename... >
   inline constexpr bool dependent_false = false;

}  // namespace TAO_JSON_PEGTL_NAMESPACE::internal

#endif
