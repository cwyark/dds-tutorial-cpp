
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
                                                    << dds::core::policy::Reliability::Reliable()
                                                    << dds::core::policy::Ownership::Exclusive();
        dds::topic::Topic<OwnershipData::Stock> topic(dp, "OwnershipStockTracker", topicQos);

        /** A dds::sub::Subscriber is created on the domain participant. */
        std::string name = "Ownership example";
        dds::sub::qos::SubscriberQos subQos
            = dp.default_subscriber_qos()
                << dds::core::policy::Partition(name);
        dds::sub::Subscriber sub(dp, subQos);

        /** The dds::sub::qos::DataReaderQos are derived from the topic qos */
        dds::sub::qos::DataReaderQos drqos = topic.qos();

        /** A dds::sub::DataReader is created on the Subscriber & Topic with the DataReaderQos. */
        dds::sub::DataReader<OwnershipData::Stock> dr(sub, topic, drqos);

        std::cout << "===[Subscriber] Ready ..." << std::endl;
        std::cout << "   Ticker   Price   Publisher   ownership strength" << std::endl;

        /** The subscriber will read messages sent by the publishers and display those with the
         * highest ownership strength. */
        bool closed = false;
        int count = 0;
        do
        {
            dds::sub::LoanedSamples<OwnershipData::Stock> samples = dr.take();
            for (dds::sub::LoanedSamples<OwnershipData::Stock>::const_iterator sample = samples.begin();
                 sample < samples.end();
                 ++sample)
            {
                if (sample->info().valid())
                {
                    if(sample->data().price() < -0.0f)
                    {
                        closed = true;
                        break;
                    }
                    std::cout << "   " << sample->data().ticker() << "     " << sample->data().price() << "   " << sample->data().publisher() << "   " << sample->data().strength() << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));
            ++count;
        }
        while (!closed && count < 1500);

        std::cout << "===[Subscriber] Market Closed" << std::endl;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = -1;
    }
    return result;
}

