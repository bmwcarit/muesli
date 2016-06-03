/*
 * #%L
 * %%
 * Copyright 2016 Randolph Voorhies and Shane Grant
 * Copyright (C) 2016 BMW Car IT GmbH
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */
#ifndef MUESLI_NAMEVALUEPAIR_H_
#define MUESLI_NAMEVALUEPAIR_H_

namespace muesli
{

  //! For holding name value pairs
  /*! This pairs a name (some string) with some value such that an archive
      can potentially take advantage of the pairing.

      In serialization functions, NameValuePairs are usually created like so:
      @code{.cpp}
      struct MyStruct
      {
        int a, b, c, d, e;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( CEREAL_NVP(a),
                   CEREAL_NVP(b),
                   CEREAL_NVP(c),
                   CEREAL_NVP(d),
                   CEREAL_NVP(e) );
        }
      };
      @endcode

      Alternatively, you can give you data members custom names like so:
      @code{.cpp}
      struct MyStruct
      {
        int a, b, my_embarrassing_variable_name, d, e;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( CEREAL_NVP(a),
                   CEREAL_NVP(b),
                   cereal::make_nvp("var", my_embarrassing_variable_name) );
                   CEREAL_NVP(d),
                   CEREAL_NVP(e) );
        }
      };
      @endcode

      There is a slight amount of overhead to creating NameValuePairs, so there
      is a third method which will elide the names when they are not used by
      the Archive:

      @code{.cpp}
      struct MyStruct
      {
        int a, b;

        template<class Archive>
        void serialize(Archive & archive)
        {
          archive( cereal::make_nvp<Archive>(a),
                   cereal::make_nvp<Archive>(b) );
        }
      };
      @endcode

      This third method is generally only used when providing generic type
      support.  Users writing their own serialize functions will normally
      explicitly control whether they want to use NVPs or not.

      @internal */
  template <class T>
  class NameValuePair
  {
    private:
      // If we get passed an array, keep the type as is, otherwise store
      // a reference if we were passed an l value reference, else copy the value
      using Type = typename std::conditional<std::is_array<typename std::remove_reference<T>::type>::value,
                                             typename std::remove_cv<T>::type,
                                             typename std::conditional<std::is_lvalue_reference<T>::value,
                                                                       T,
                                                                       typename std::decay<T>::type>::type>::type;

//      // prevent nested nvps
//      static_assert( !joynr::util::IsDerivedFromTemplate<NameValuePair, T>::value,
//                     "Cannot pair a name to a NameValuePair" );

      NameValuePair & operator=( NameValuePair const & ) = delete;

    public:
      //! Constructs a new NameValuePair
      /*! @param n The name of the pair
          @param v The value to pair.  Ideally this should be an l-value reference so that
                   the value can be both loaded and saved to.  If you pass an r-value reference,
                   the NameValuePair will store a copy of it instead of a reference.  Thus you should
                   only pass r-values in cases where this makes sense, such as the result of some
                   size() call.
          @internal */
      NameValuePair( char const * n, T && v ) : name(n), value(std::forward<T>(v)) {}

      char const * name;
      Type value;
  };

  // ######################################################################
  //! Creates a name value pair
  /*! @relates NameValuePair
      @ingroup Utility */
  template <class T> inline
  NameValuePair<T> make_nvp( std::string const & name, T && value )
  {
    return {name.c_str(), std::forward<T>(value)};
  }

  //! Creates a name value pair
  /*! @relates NameValuePair
      @ingroup Utility */
  template <class T> inline
  NameValuePair<T> make_nvp( const char * name, T && value )
  {
    return {name, std::forward<T>(value)};
  }
} // namespace muesli

//! Creates a name value pair for the variable T with the same name as the variable
/*! @relates NameValuePair
    @ingroup Utility */
#define MUESLI_NVP(T) ::muesli::make_nvp(#T, T)


#endif // MUESLI_NAMEVALUEPAIR_H_
