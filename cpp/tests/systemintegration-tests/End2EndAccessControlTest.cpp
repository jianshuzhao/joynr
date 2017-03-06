/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2017 BMW Car IT GmbH
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

#include <chrono>
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "joynr/DispatcherUtils.h"
#include "joynr/Settings.h"
#include "joynr/tests/testProxy.h"

#include "runtimes/cluster-controller-runtime/JoynrClusterControllerRuntime.h"

#include "tests/utils/LibJoynrMockObjects.h"

using namespace ::testing;
using namespace joynr;

ACTION_P(ReleaseSemaphore, semaphore){
    semaphore->notify();
}

void removeFileInCurrentDirectory(const std::string& filePattern) {
    boost::filesystem::path currentDir(".");
    boost::regex pattern(filePattern);
    boost::filesystem::directory_iterator dirBegin(currentDir), dirEnd;

    BOOST_FOREACH(const boost::filesystem::path& file, std::make_pair(dirBegin, dirEnd))
    {
        if(boost::filesystem::is_regular_file(file))
        {
            std::string fileName = file.filename().string();
            boost::smatch result;
            if(boost::regex_match(fileName, result, pattern)){
                boost::filesystem::remove(file);
            }
        }
    }
}

class End2EndAccessControlTest : public TestWithParam<std::tuple<std::string>> {
public:
    End2EndAccessControlTest() :
        runtimeAcON(nullptr),
        runtimeAcOFF(nullptr),
        testProvider(nullptr),
        testProxy(nullptr),
        domain("End2EndAccessControlTest"),
        providerParticipantId(),
        semaphore(0),
        AC_ENTRIES_FILE("AccessControl.entries"),
        MESSAGINGQOS_TTL(1000)
    {
    }

    void init(std::string pathToACCentriesFile) {
        // copy access entry file to bin folder for the test so that runtimes will find and load the file
        {
            std::ofstream dst(AC_ENTRIES_FILE.c_str());
            std::ifstream src(pathToACCentriesFile);
            dst << src.rdbuf();
        }

        auto settings1 = std::make_unique<Settings>("test-resources/MessagingWithAccessControlEnabled.settings");
        auto settings2 = std::make_unique<Settings>("test-resources/MessagingWithAccessControlDisabled.settings");

        runtimeAcON =  JoynrRuntime::createRuntime(std::move(settings1));
        runtimeAcOFF = JoynrRuntime::createRuntime(std::move(settings2));

        // Create a provider on runtimeAcON
        types::ProviderQos providerQos;
        providerQos.setScope(joynr::types::ProviderScope::GLOBAL);

        testProvider = std::make_shared<MockTestProvider>();
        providerParticipantId = runtimeAcON->registerProvider<tests::testProvider>(domain, testProvider, providerQos);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Create a proxy on runtimeAcOFF
        auto testProxyBuilder = runtimeAcOFF->createProxyBuilder<tests::testProxy>(domain);
        DiscoveryQos discoveryQos;
        discoveryQos.setArbitrationStrategy(DiscoveryQos::ArbitrationStrategy::HIGHEST_PRIORITY);
        discoveryQos.setDiscoveryTimeoutMs(1000);

        testProxy = testProxyBuilder->setMessagingQos(MessagingQos(MESSAGINGQOS_TTL))
                                                   ->setDiscoveryQos(discoveryQos)
                                                   ->build();
    }

    ~End2EndAccessControlTest() override {
        runtimeAcON->unregisterProvider(providerParticipantId);

        runtimeAcON.reset();
        runtimeAcOFF.reset();

        // Delete test specific files
        removeFileInCurrentDirectory(".*\\.settings");
        removeFileInCurrentDirectory(".*\\.persist");
        std::remove(AC_ENTRIES_FILE.c_str());
    }

protected:
    std::unique_ptr<JoynrRuntime> runtimeAcON;
    std::unique_ptr<JoynrRuntime> runtimeAcOFF;

    std::shared_ptr<MockTestProvider> testProvider;
    std::unique_ptr<tests::testProxy> testProxy;

    std::string domain;
    std::string providerParticipantId;
    joynr::Semaphore semaphore;

    const std::string AC_ENTRIES_FILE;
    const std::int64_t MESSAGINGQOS_TTL;
};

TEST_F(End2EndAccessControlTest, DISABLED_proxyDoesNotHavePermission) {

    init("test-resources/AccessControlYesPermissions.entries");

    // If AccessControl is active, the proxy cannot call methodWithNoInputParameters (see AC_ENTRIES_FILE file)
    EXPECT_CALL(*testProvider, methodWithNoInputParametersMock(_,_))
            .Times(0);

    std::int32_t outputParameter = 0;

    // side effect of calling a method without access permission
    EXPECT_THROW(testProxy->methodWithNoInputParameters(outputParameter),
                 exceptions::JoynrTimeOutException);
}

TEST_F(End2EndAccessControlTest, DISABLED_proxyDoesHavePermission) {

    init("test-resources/AccessControlNoPermissions.entries");

    EXPECT_CALL(*testProvider, methodWithNoInputParametersMock(_,_))
            .Times(1)
            .WillOnce(ReleaseSemaphore(&semaphore));

    std::int32_t outputParameter = 0;
    testProxy->methodWithNoInputParameters(outputParameter);

    EXPECT_TRUE(semaphore.waitFor(std::chrono::milliseconds(MESSAGINGQOS_TTL)));
}
