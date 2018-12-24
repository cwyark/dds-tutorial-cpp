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
#include "gen/LifecycleData_DCPS.hpp"

std::string sampleStateToString(const dds::sub::status::SampleState& state)
{
    std::string stateString = "INVALID_STATE";
    if(state == dds::sub::status::SampleState::read())
    {
        stateString = "READ_SAMPLE_STATE";
    }
    else if(state == dds::sub::status::SampleState::not_read())
    {
        stateString = "NOT_READ_SAMPLE_STATE";
    }
    return stateString;
}

std::string viewStateToString(const dds::sub::status::ViewState& state)
{
    std::string stateString = "INVALID_STATE";
    if(state == dds::sub::status::ViewState::new_view())
    {
        stateString = "NEW_VIEW_STATE";
    }
    else if(state == dds::sub::status::ViewState::not_new_view())
    {
        stateString = "NOT_NEW_VIEW_STATE";
    }
    else if(state == dds::sub::status::ViewState::any())
    {
        stateString = "ANY_VIEW_STATE";
    }
    return stateString;
}

std::string instanceStateToString(const dds::sub::status::InstanceState& state)
{
    std::string stateString = "INVALID_STATE";
    if(state == dds::sub::status::InstanceState::alive())
    {
        stateString = "ALIVE_INSTANCE_STATE";
    }
    else if(state == dds::sub::status::InstanceState::not_alive_disposed())
    {
        stateString = "NOT_ALIVE_DISPOSED_INSTANCE_STATE";
    }
    else if(state == dds::sub::status::InstanceState::not_alive_no_writers())
    {
        stateString = "NOT_ALIVE_NO_WRITERS_INSTANCE_STATE";
    }
    else if(state == dds::sub::status::InstanceState::any())
    {
        stateString = "ANY_INSTANCE_STATE";
    }
    return stateString;
}

void usagePub()
{
    std::cerr << "*** ERROR" << std::endl;
    std::cerr << "*** usage : publisher <autodispose_flag> <writer_action>" << std::endl;
    std::cerr << "***         . autodispose_flag = false | true" << std::endl;
    std::cerr << "***         . writer_action = dispose | unregister | stoppub" << std::endl;
}

void writeSample(std::string writer_action, dds::pub::DataWriter<LifecycleData::Msg> dw)
{
    /** A sample is created and then written. */
    LifecycleData::Msg msgInstance;
    if(writer_action == "dispose")
    {
        msgInstance.userID() = 1;
        msgInstance.message() = "Lifecycle_1";
        msgInstance.writerStates() = "SAMPLE_SENT -> INSTANCE_DISPOSED -> DATAWRITER_DELETED";
    }
    else if(writer_action == "unregister")
    {
        msgInstance.userID() = 2;
        msgInstance.message() = "Lifecycle_2";
        msgInstance.writerStates() = "SAMPLE_SENT -> INSTANCE_UNREGISTERED -> DATAWRITER_DELETED";
    }
    else if(writer_action == "stoppub")
    {
        msgInstance.userID() = 3;
        msgInstance.message() = "Lifecycle_3";
        msgInstance.writerStates() = "SAMPLE_SENT -> DATAWRITER_DELETED";
    }

    std::cout << "=== [Publisher] written a message containing :" << std::endl;
    std::cout << "    userID        : " << msgInstance.userID() << std::endl;
    std::cout << "    Message       : \"" << msgInstance.message() << "\"" << std::endl;
    std::cout << "    writerStates  : \"" << msgInstance.writerStates() << "\"" << std::endl;
    dw << msgInstance;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "=== [Publisher]  : SAMPLE_SENT" << std::endl;

    /** The sample instance is then either disposed of or unregistered depending on user preference */
    if(writer_action == "dispose")
    {
        dw.dispose_instance(dw.lookup_instance(msgInstance));
    }
    else if(writer_action == "unregister")
    {
        dw.unregister_instance(dw.lookup_instance(msgInstance));
    }
}

int main (int argc, char *argv[])
{
    int result = 0;

    if(argc < 3 ||
        ((strcmp(argv[1], "false") != 0) &&
        (strcmp(argv[1], "true") != 0) &&
        (strcmp(argv[2], "dispose") != 0) &&
        (strcmp(argv[2], "unregister") != 0) &&
        (strcmp(argv[2], "stoppub") != 0)))
    {
        usagePub();
        return -1;
    }
    bool autodispose_unregistered_instances = (strcmp(argv[1], "true") == 0);
    std::string writer_action = argv[2];

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
                << dds::core::policy::Reliability::Reliable();

        /** A dds::topic::Topic is created for our sample type on the domain participant. */
        dds::topic::Topic<LifecycleData::Msg> topic(dp, "LifecycleData_Msg", topicQos);

        /** A dds::pub::Publisher is created on the domain participant. */
        std::string name = "Lifecycle example";
        dds::pub::qos::PublisherQos pubQos
            = dp.default_publisher_qos()
                << dds::core::policy::Partition(name);
        dds::pub::Publisher pub(dp, pubQos);

        /** The dds::pub::qos::DataWriterQos is derived from the topic qos and autodispose is
         * set to either manual or automatic based on the users preference. This is so the publisher
         * can optionally be run (and exit) before the subscriber. It prevents or allows the middleware
         * default 'clean up' of the topic instance after the writer deletion, this deletion
         * implicitly performs DataWriter::unregister_instance */
        dds::pub::qos::DataWriterQos dwqos = topic.qos();
        dwqos << dds::core::policy::WriterDataLifecycle(autodispose_unregistered_instances);

        /** A dds::pub::DataWriter to write messages and another to indicate when to stop the example are
         *  created on the Publisher & Topic with the modififed Qos. */
        dds::pub::DataWriter<LifecycleData::Msg> dw(pub, topic, dwqos);
        dds::pub::DataWriter<LifecycleData::Msg> dwStopper(pub, topic, dwqos);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        writeSample(writer_action, dw);

        std::cout << "=== [Publisher] waiting 500ms to let the subscriber handle the previous write state ..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /** The dds::pub::DataWriter is reassigned to a null writer to force deletion */
        dw = dds::pub::DataWriter<LifecycleData::Msg>(dds::core::null);
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /** A message is sent to stop the subscriber */
        LifecycleData::Msg msgInstance(4, "Lifecycle_4", "STOPPING_SUBSCRIBER");
        std::cout << "=== [Publisher]   :" << std::endl;
        std::cout << "    userID        : " << msgInstance.userID() << std::endl;
        std::cout << "    Message       : \"" << msgInstance.message() << "\"" << std::endl;
        std::cout << "    writerStates  : \"" << msgInstance.writerStates() << "\"" << std::endl;
        dwStopper << msgInstance;

        /* A sleep ensures time is allowed for the sample to be written to the network.
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
