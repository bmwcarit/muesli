/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2016 BMW Car IT GmbH
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

#ifndef MUESLI_ARCHIVEREGISTRY_H_
#define MUESLI_ARCHIVEREGISTRY_H_

namespace muesli
{

/** this type sequence can be extended/modified through specialization:
 *  @code
 *  #include <boost/mpl/joint_view.hpp>
 *
 *  template<>
 *  struct ExtensibleTypeSequence<MyTypeList>
 *  {
 *      using AdditionalTypes = boost::mpl::vector<Foo, Bar>;
 *      using type = boost::mpl::joint_view<AdditionalTypes, MyTypeList::type>;
 *  };
 *  @endcode
 */
template <typename Sequence>
struct ExtensibleTypeSequence
{
    using type = Sequence;
};

class JsonOutputArchive;
class JsonInputArchive;

struct RegisterJsonArchive
{
    using OutputArchive = JsonOutputArchive;
    using InputArchive = JsonInputArchive;
};

using RegisteredArchives = boost::mpl::vector<RegisterJsonArchive>;

} // namespace muesli

#endif // MUESLI_ARCHIVEREGISTRY_H_
