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

#ifndef MUESLI_SKIPINTROOUTROWRAPPER_H_
#define MUESLI_SKIPINTROOUTROWRAPPER_H_

namespace muesli
{

// wrapper which tells the archive to serialize the wrapped element WITHOUT calling intro/outro
template <typename Wrapped>
struct SkipIntroOutroWrapper
{
    explicit SkipIntroOutroWrapper(const Wrapped* wrapped) : _wrapped(const_cast<Wrapped*>(wrapped))
    {
    }

    Wrapped* _wrapped;
};

} // namespace muesli

#endif // MUESLI_SKIPINTROOUTROWRAPPER_H_
