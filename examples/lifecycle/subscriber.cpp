
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

int main (int argc, char *argv[])
{
    int result = 0;
    try
    {
        /** A domain participant and topic are created identically as in
         the ::publisher */
        dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());
        dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                                                    << dds::core::policy::Durability::Transient()
                                                    << dds::core::policy::Reliability::Reliable();
        dds::topic::Topic<LifecycleData::Msg> topic(dp, "LifecycleData_Msg", topicQos);

        /** A dds::sub::Subscriber is created on the domain participant. */
        std::string name = "Lifecycle example";
        dds::sub::qos::SubscriberQos subQos
            = dp.default_subscriber_qos()
                << dds::core::policy::Partition(name);
        dds::sub::Subscriber sub(dp, subQos);

        /** The dds::sub::qos::DataReaderQos are derived from the topic qos */
        dds::sub::qos::DataReaderQos drqos = topic.qos();

        /** A dds::sub::DataReader is created on the Subscriber & Topic with the DataReaderQos. */
        dds::sub::DataReader<LifecycleData::Msg> dr(sub, topic, drqos);

        std::cout << "=== [Subscriber] Ready ..." << std::endl;

        /** An attempt to take samples is made repeatedly until it succeeds,
         * or one hundred iterations have occurred. */
        bool closed = false;
        int count = 0;
        int maxCount = 100;
        do
        {
            dds::sub::LoanedSamples<LifecycleData::Msg> samples = dr.read();
            for (dds::sub::LoanedSamples<LifecycleData::Msg>::const_iterator sample = samples.begin();
                 sample < samples.end();
                 ++sample)
            {
                std::cout << "=== [Subscriber] message received :" << std::endl;
                std::cout << "    userID        : " << sample->data().userID() << std::endl;
                std::cout << "    Message       : \"" << sample->data().message() << "\"" << std::endl;
                std::cout << "    writerStates  : \"" << sample->data().writerStates() << "\"" << std::endl;
                std::cout << "    valid_data    : " << sample->info().valid() << std::endl;
                std::cout << "    sample_state:" << sampleStateToString(sample->info().state().sample_state());
                std::cout << "-view_state:" << viewStateToString(sample->info().state().view_state());
                std::cout << "-instance_state:" << instanceStateToString(sample->info().state().instance_state()) << std::endl;
                closed = (sample->data().writerStates() == "STOPPING_SUBSCRIBER");
                ++count;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        while (!closed && count < maxCount);

        std::cout << "=== [Subscriber] stopping after " << count << " iterations - closed = " << closed << std::endl;
        if (count == maxCount)
        {
            std::cout << "*** Error : max " << maxCount << " iterations reached" << std::endl;
            result = 1;
        }
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = 1;
    }
    return result;
}
