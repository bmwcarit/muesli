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

#include <sstream>

#include <benchmark/benchmark.h>

#include "muesli/archives/json/JsonInputArchive.h"
#include "muesli/archives/json/JsonOutputArchive.h"

#include "muesli/streams/StdIStreamWrapper.h"
#include "muesli/streams/StdOStreamWrapper.h"

#include "muesli/streams/StringIStream.h"
#include "muesli/streams/StringOStream.h"

#include "../unit-tests/testtypes/TStructExtended.h"

void benchmarkJsonOutputArchiveStdOStreamWrapper(benchmark::State& state)
{
    using OutputStreamImpl = muesli::StdOStreamWrapper<std::ostream>;
    using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;

    muesli::tests::testtypes::TStructExtended data;

    while (state.KeepRunning()) {
        std::stringstream stream;
        OutputStreamImpl outputStreamWrapper(stream);
        JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
        jsonOutputArchive(data);
        benchmark::DoNotOptimize(stream.str());
    }
}

void benchmarkJsonOutputArchiveStringOStream(benchmark::State& state)
{
    using OutputStreamImpl = muesli::StringOStream;
    using JsonOutputArchiveImpl = muesli::JsonOutputArchive<OutputStreamImpl>;

    muesli::tests::testtypes::TStructExtended data;

    while (state.KeepRunning()) {
        OutputStreamImpl outputStreamWrapper;
        JsonOutputArchiveImpl jsonOutputArchive(outputStreamWrapper);
        jsonOutputArchive(data);
        benchmark::DoNotOptimize(outputStreamWrapper.getString());
    }
}

BENCHMARK(benchmarkJsonOutputArchiveStdOStreamWrapper);
BENCHMARK(benchmarkJsonOutputArchiveStringOStream);

BENCHMARK_MAIN()
