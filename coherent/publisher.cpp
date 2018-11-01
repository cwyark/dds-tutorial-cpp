/*              ADLINK Advanced Robotic Platfrom Group
 * Author: Ewing Kang (ewing.kang@adlinktech.com)
 * 
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
#include "gen/CoherentData_DCPS.hpp"

void usagePub()
{
    std::cerr << "*** ERROR" << std::endl;
    std::cerr << "*** usage : publisher <pubA_name> <pubB_name> <topic_pub_interval> <sample_pub_interval> <max_iterations> <coherent_choice>" << std::endl;
    std::cerr << "***         . publisher_name_A" << std::endl;
	std::cerr << "***         . publisher_name_B" << std::endl;
    std::cerr << "***         . topic_pub_interval" << std::endl;
	std::cerr << "***         . sample_pub_interval" << std::endl;
	std::cerr << "***         . iter_interval" << std::endl;
    std::cerr << "***         . max_iterations" << std::endl;
	std::cerr << "***         . coherent_choice" << std::endl;
}

int main (int argc, char *argv[])
{
    int result = 0;

    if(argc != 8)
    {
        usagePub();
        return -1;
    }
    std::string publisher_nameA = argv[1];
	std::string publisher_nameB = argv[2];
    float topic_pub_interval = atof(argv[3]);
	float sample_pub_interval = atof(argv[4]);
	float iter_interval = atof(argv[5]);
    int max_iterations = atoi(argv[6]);
	int coherent_choice = atoi(argv[7]);

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
        dds::topic::Topic<CoherentData::Stock> topicA(dp, "CoherentTopicA", topicQos);
		dds::topic::Topic<CoherentData::Stock> topicB(dp, "CoherentTopicB", topicQos);

        /** A dds::pub::Publisher is created on the domain participant. */
        std::string name = "Coherent example";
        dds::pub::qos::PublisherQos pubQos
            = dp.default_publisher_qos()
                << dds::core::policy::Partition(name);
		std::string coherent_name;
		switch (coherent_choice) {
		  case 1 : 
			pubQos << dds::core::policy::Presentation::InstanceAccessScope(true, false);
			coherent_name = "Instance";
			break;
		  case 2 :
			pubQos << dds::core::policy::Presentation::TopicAccessScope(true, false);
			coherent_name = "Topic";
			break;
		  case 3 :
			pubQos << dds::core::policy::Presentation::GroupAccessScope(true, false);
			coherent_name = "Group";
			break;
		  default: 
			coherent_name = "No coherent"; 
			break;
		}
				
				
        dds::pub::Publisher pub(dp, pubQos);

        /** The dds::pub::qos::DataWriterQos is derived from the topic qos and the
         * WriterDataLifecycle::ManuallyDisposeUnregisteredInstances policy is
         * specified as an addition. This is so the publisher can optionally be run (and
         * exit) before the subscriber. It prevents the middleware default 'clean up' of
         * the topic instance after the writer deletion, this deletion implicitly performs
         * DataWriter::unregister_instance */
        dds::pub::qos::DataWriterQos dwqos = topicA.qos();
		dwqos << dds::core::policy::History::KeepAll();		// So the coherent will work properly

        /** A dds::pub::DataWriter is created on the Publisher & Topic with the modififed Qos. */
        dds::pub::DataWriter<CoherentData::Stock> dwA(pub, topicA, dwqos);
		dds::pub::DataWriter<CoherentData::Stock> dwB(pub, topicB, dwqos);

        std::cout << "===== [Publisher] =====" << std::endl;
		std::cout << "Publishing two topics: " << publisher_nameA <<" and " << publisher_nameB << " with interval between Topics: " << topic_pub_interval << "sec" << std::endl;
		std::cout << "Each topics will have two samples, with interval " << sample_pub_interval << "sec" << std::endl;
        std::cout << "Total iterations: " << max_iterations << " ; Coherent type: " << coherent_name << std::endl;
		std::cout << "=== publishing ===" << std::endl;

        /** Samples are created and then written. */
        CoherentData::Stock msgInstanceA("TopicInstanceA", 10.0f, publisher_nameA.c_str(), topic_pub_interval);
		CoherentData::Stock msgInstanceB("TopicInstanceB", 10.0f, publisher_nameB.c_str(), topic_pub_interval);
        dds::core::InstanceHandle ihA = dwA.register_instance(msgInstanceA);
		dds::core::InstanceHandle ihB = dwB.register_instance(msgInstanceB);

        for (int i = 0; i < max_iterations; i++)
        {
			std::cout << "-Tx cycle: " << i << std::endl;
			if(coherent_choice) 
			{
				dds::pub::CoherentSet coh_set(pub);	
				
				dwA.write(msgInstanceA, ihA);
				msgInstanceA.price() += 0.5f;
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*sample_pub_interval)));
				dwA.write(msgInstanceA, ihA);
				msgInstanceA.price() += 0.5f;
				
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*topic_pub_interval)));
				
				dwB.write(msgInstanceB, ihB);
				msgInstanceB.price() += 0.5f;
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*sample_pub_interval)));
				dwB.write(msgInstanceB, ihB);
				msgInstanceB.price() += 0.5f;
				coh_set.end();
			}else 
			{
				std::cout << " unsynced: " << std::endl;
				dwA.write(msgInstanceA, ihA);
				msgInstanceA.price() += 0.5f;
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*sample_pub_interval)));
				dwA.write(msgInstanceA, ihA);
				msgInstanceA.price() += 0.5f;
				
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*topic_pub_interval)));
				
				dwB.write(msgInstanceB, ihB);
				msgInstanceB.price() += 0.5f;
				std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*sample_pub_interval)));
				dwB.write(msgInstanceB, ihB);
				msgInstanceB.price() += 0.5f;
			}
			
			std::this_thread::sleep_for(std::chrono::milliseconds( (int)(1000*iter_interval)));
        }
        
        /** The subscriber is stopped if specified by the user */

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
