/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to  PrismTech
 *   Limited, its affiliated companies and licensors. All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "gen/OwnershipData_DCPS.hpp"

void usagePub()
{
    std::cerr << "*** ERROR" << std::endl;
    std::cerr << "*** usage : publisher <publisher_name> <ownership_strength> <max_iterations> <stop_subscriber_flag>" << std::endl;
    std::cerr << "***         . publisher_name" << std::endl;
    std::cerr << "***         . ownership_strength" << std::endl;
    std::cerr << "***         . max_iterations" << std::endl;
    std::cerr << "***         . stop_subscriber_flag = 0 | 1" << std::endl;
}

int main (int argc, char *argv[])
{
    int result = 0;

    if(argc < 5 || (strcmp(argv[4], "0") && strcmp(argv[4], "1")))
    {
        usagePub();
        return -1;
    }
    std::string publisher_name = argv[1];
    int ownership_strength = atoi(argv[2]);
    int max_iterations = atoi(argv[3]);
    bool stop_subscriber_flag = (strcmp(argv[4], "1") == 0);

    try
    {
        /** A dds::domain::DomainParticipant is created for the default domain. */
        dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

        /** The Durability::Transient policy is specified as a dds::topic::qos::TopicQos
         * so that even if the subscriber does not join until after the sample is written
         * then the DDS will still retain the sample for it. The Reliability::Reliable
         * policy is also specified to guarantee delivery. */
        dds::topic::qos::TopicQos topicQos
             = dp.default_topic_qos()
                << dds::core::policy::Durability::Transient()
                << dds::core::policy::Reliability::Reliable()
                << dds::core::policy::Ownership::Exclusive();

        /** A dds::topic::Topic is created for our sample type on the domain participant. */
        dds::topic::Topic<OwnershipData::Stock> topic(dp, "OwnershipStockTracker", topicQos);

        /** A dds::pub::Publisher is created on the domain participant. */
        std::string name = "Ownership example";
        dds::pub::qos::PublisherQos pubQos
            = dp.default_publisher_qos()
                << dds::core::policy::Partition(name);
        dds::pub::Publisher pub(dp, pubQos);

        /** The dds::pub::qos::DataWriterQos is derived from the topic qos and the
         * WriterDataLifecycle::ManuallyDisposeUnregisteredInstances policy is
         * specified as an addition. This is so the publisher can optionally be run (and
         * exit) before the subscriber. It prevents the middleware default 'clean up' of
         * the topic instance after the writer deletion, this deletion implicitly performs
         * DataWriter::unregister_instance */
        dds::pub::qos::DataWriterQos dwqos = topic.qos();
        dwqos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances()
            << dds::core::policy::OwnershipStrength(ownership_strength);

        /** A dds::pub::DataWriter is created on the Publisher & Topic with the modififed Qos. */
        dds::pub::DataWriter<OwnershipData::Stock> dw(pub, topic, dwqos);

        std::cout << "=== [Publisher] Publisher " << publisher_name << " with strength : " << ownership_strength;
        std::cout << " / sending " << max_iterations << " prices ..." << " stop_subscriber_flag=" << argv[4] << std::endl;

        /** Samples are created and then written. */
        OwnershipData::Stock msgInstance("MSFT", 10.0f, publisher_name.c_str(), ownership_strength);
        dds::core::InstanceHandle ih = dw.register_instance(msgInstance);

        for (int i = 0; i < max_iterations; i++)
        {
            dw.write(msgInstance, ih);
            std::this_thread::sleep_for(std::chrono::seconds(2));
            msgInstance.price() += 0.5f;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /** The subscriber is stopped if specified by the user */
        if (stop_subscriber_flag)
        {
            msgInstance.price() = -1.0f;
            std::cout << "=== Stopping the subscriber" << std::endl;
            dw.write(msgInstance, ih);
        }

        /* A short sleep ensures time is allowed for the sample to be written to the network.
        If the example is running in *Single Process Mode* exiting immediately might
        otherwise shutdown the domain services before this could occur */
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}
