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

#ifndef MUESLI_STREAMREGISTRY_H_
#define MUESLI_STREAMREGISTRY_H_

#include "muesli/detail/IncrementalTypeList.h"
#include "muesli/Tags.h"

#define MUESLI_REGISTER_STREAM(Tag, Stream) MUESLI_ADD_TO_INCREMENTAL_TYPELIST(Tag, Stream)

#define MUESLI_REGISTER_INPUT_STREAM(Stream)                                                       \
    MUESLI_REGISTER_STREAM(muesli::tags::InputStream, Stream)

#define MUESLI_REGISTER_OUTPUT_STREAM(Stream)                                                      \
    MUESLI_REGISTER_STREAM(muesli::tags::OutputStream, Stream)

#endif // MUESLI_STREAMREGISTRY_H_
