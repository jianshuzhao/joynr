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
#include "joynr/exceptions/NoCompatibleProviderFoundException.h"

namespace joynr
{

namespace exceptions
{

const std::string NoCompatibleProviderFoundException::TYPE_NAME =
        "io.joynr.exceptions.NoCompatibleProviderFoundException";

static const bool isNoCompatibleProviderFoundExceptionRegistered =
        Variant::registerType<joynr::exceptions::NoCompatibleProviderFoundException>(
                NoCompatibleProviderFoundException::TYPE_NAME);

NoCompatibleProviderFoundException::NoCompatibleProviderFoundException(
        const std::unordered_set<joynr::types::Version>& discoveredIncompatibleVersions) noexcept
        : DiscoveryException(),
          discoveredIncompatibleVersions(discoveredIncompatibleVersions)
{
    message = "Unable to find a provider with a compatible version. " +
              std::to_string(discoveredIncompatibleVersions.size()) +
              " incompabible versions found:";
    for (const auto& version : discoveredIncompatibleVersions) {
        message += " " + version.toString();
    }
}

const std::string& NoCompatibleProviderFoundException::getTypeName() const
{
    return NoCompatibleProviderFoundException::TYPE_NAME;
}

const std::unordered_set<joynr::types::Version>& NoCompatibleProviderFoundException::
        getDiscoveredIncompatibleVersions() const
{
    return discoveredIncompatibleVersions;
}

NoCompatibleProviderFoundException* NoCompatibleProviderFoundException::clone() const
{
    return new NoCompatibleProviderFoundException(
            const_cast<NoCompatibleProviderFoundException&>(*this));
}

} // namespace exceptions

} // namespace joynr
