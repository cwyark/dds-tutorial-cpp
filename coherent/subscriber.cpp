/*                     ADLINK Advanced Robotic Platfrom Group
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
#include <iomanip>
#include <string>
#include <thread>
#include <functional>
#include <chrono>
#include "gen/CoherentData_DCPS.hpp"

void topic_subA( dds::core::cond::WaitSet &,
				 dds::core::cond::StatusCondition &,
				 dds::sub::DataReader<CoherentData::Stock> &);

void topic_subB( dds::core::cond::WaitSet &, 
				 dds::core::cond::StatusCondition &,
				 dds::sub::DataReader<CoherentData::Stock> &);

void usagePub();

static int clamp(int, int, int);

int main (int argc, char *argv[])
{
    int result = 0;
	
	if(argc != 2)
    {
        usagePub();
        return -1;
    }
    int coherent_choice = atoi(argv[1]);
	clamp(coherent_choice, 0, 3);
	
	
    try
    {
        /** A domain participant and topic are created identically as in
         the ::publisher */
        dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());
        dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                                                    << dds::core::policy::Durability::Transient()
                                                    << dds::core::policy::Reliability::Reliable();
                                                    // << dds::core::policy::Ownership::Exclusive();


        dds::topic::Topic<CoherentData::Stock> topicA(dp, "CoherentTopicA", topicQos);
		dds::topic::Topic<CoherentData::Stock> topicB(dp, "CoherentTopicB", topicQos);

        /** A dds::sub::Subscriber is created on the domain participant. */
        std::string name = "Coherent example";
        dds::sub::qos::SubscriberQos subQos
            = dp.default_subscriber_qos()
                << dds::core::policy::Partition(name);
				
		switch (coherent_choice) {
		  case 1 : 
			subQos << dds::core::policy::Presentation::InstanceAccessScope(true, false);
			break;
		  case 2 :
			subQos << dds::core::policy::Presentation::TopicAccessScope(true, false);
			break;
		  case 3 :
			subQos << dds::core::policy::Presentation::GroupAccessScope(true, false);
			break;
		  default:break;
		}
				
        dds::sub::Subscriber sub(dp, subQos);

        /** The dds::sub::qos::DataReaderQos are derived from the topic qos */
        dds::sub::qos::DataReaderQos drqos = topicA.qos();
		drqos << dds::core::policy::History::KeepAll();

        /** A dds::sub::DataReader is created on the Subscriber & Topic with the DataReaderQos. */
        dds::sub::DataReader<CoherentData::Stock> drA(sub, topicA, drqos);
		dds::sub::DataReader<CoherentData::Stock> drB(sub, topicB, drqos);

		// Create a Condition to attach to a Waitset
		dds::core::cond::StatusCondition readerSCA = dds::core::cond::StatusCondition(drA);
		dds::core::cond::StatusCondition readerSCB = dds::core::cond::StatusCondition(drB);
		readerSCA.enabled_statuses(dds::core::status::StatusMask::data_available());
		readerSCB.enabled_statuses(dds::core::status::StatusMask::data_available());
		// Create WaitSet and attach Condition
		dds::core::cond::WaitSet waitsetA, waitsetB;
		waitsetA.attach_condition(readerSCA); // or waitset += readerSC;
		waitsetB.attach_condition(readerSCB); 
		
				
        std::cout << "===[Subscriber] Ready and waiting..." << std::endl;
        std::cout << "   Topic   Price   Publisher   ownership strength" << std::endl;

		std::thread thrd_subA(topic_subA, std::ref(waitsetA), std::ref(readerSCA), std::ref(drA) );
		std::thread thrd_subB(topic_subB, std::ref(waitsetB), std::ref(readerSCB), std::ref(drB) );
        /** The subscriber will read messages sent by the publishers and display those with the
         * highest ownership strength. */
        /*bool closed = false;
        int count = 0;
        do
        {
			// wait set for topic A 
			conditions = waitsetA.wait();
			for (uint i=0; i < conditions.size(); i++)
			{
				if (conditions[i] != readerSCA) {
					std::cerr << "Something triggered the waitSet" << std::endl;
					continue;
				}
				dds::sub::LoanedSamples<CoherentData::Stock> samplesA = drA.take();
				for (dds::sub::LoanedSamples<CoherentData::Stock>::const_iterator sampleA = samplesA.begin();
					sampleA < samplesA.end();
					++sampleA)
				{
					if (sampleA->info().valid())
					{
						if(sampleA->data().price() < -0.0f)
						{
							closed = true;
							break;
						}
						std::cout  << std::fixed << std::setprecision(1) 
								   << "   " << sampleA->data().ticker() << "   " << sampleA->data().price() 
								   << "   " << sampleA->data().publisher() << "   " << sampleA->data().strength() << std::endl;
					}
				}
				//std::this_thread::sleep_for(std::chrono::seconds(2));
				++count;
			}
			
			// wait for topic B 
			conditions = waitsetB.wait();
			for (uint i=0; i < conditions.size(); i++)
			{
				if (conditions[i] != readerSCB) {
					std::cerr << "Something triggered the waitSet" << std::endl;
					continue;
				}
				dds::sub::LoanedSamples<CoherentData::Stock> samplesB = drB.take();
				for (dds::sub::LoanedSamples<CoherentData::Stock>::const_iterator sampleB = samplesB.begin();
					sampleB < samplesB.end();
					++sampleB)
				{
					if (sampleB->info().valid())
					{
						if(sampleB->data().price() < -0.0f)
						{
							closed = true;
							break;
						}
						std::cout  << std::fixed << std::setprecision(1) 
								   << "   " << sampleB->data().ticker() << "   " << sampleB->data().price() 
								   << "   " << sampleB->data().publisher() << "   " << sampleB->data().strength() << std::endl;
					}
				}
				//std::this_thread::sleep_for(std::chrono::seconds(2));
				++count;
			}
        }
        while (!closed && count < 1500);*/
		
		thrd_subA.join();
		thrd_subB.join();

        std::cout << "===[Subscriber] Market Closed" << std::endl;
    }
    catch (const dds::core::Exception& e)
    {
        std::cerr << "ERROR: Exception: " << e.what() << std::endl;
        result = -1;
    }
    return result;
}

void topic_subA( dds::core::cond::WaitSet& waitsetA, 
				 dds::core::cond::StatusCondition& readerSCA,
				 dds::sub::DataReader<CoherentData::Stock>& drA)
{
	bool closed = false;
	int count = 0;
	dds::core::cond::WaitSet::ConditionSeq conditions;
	do
	{
		/* wait set for topic A */
		conditions = waitsetA.wait();
		std::cout  << "== A call ==" << std::endl;
		for (uint i=0; i < conditions.size(); i++)
		{
			if (conditions[i] != readerSCA) {
				std::cerr << "Something triggered the waitSet" << std::endl;
				continue;
			}
			dds::sub::LoanedSamples<CoherentData::Stock> samplesA = drA.take();
			for (dds::sub::LoanedSamples<CoherentData::Stock>::const_iterator sampleA = samplesA.begin();
				sampleA < samplesA.end();
				++sampleA)
			{
				if (sampleA->info().valid())
				{
					if(sampleA->data().price() < -0.0f)
					{
						closed = true;
						break;
					}
					std::cout  << std::fixed << std::setprecision(1) 
								<< "   " << sampleA->data().ticker() << "   " << sampleA->data().price() 
								<< "   " << sampleA->data().publisher() << "   " << sampleA->data().strength() << std::endl;
				}
			}
			std::cout  << "   -- loop A: " << count << " --" << std::endl;
			++count;
		}
	}
	while (!closed && count < 1500);
}

void topic_subB( dds::core::cond::WaitSet& waitsetB, 
				 dds::core::cond::StatusCondition& readerSCB,
				 dds::sub::DataReader<CoherentData::Stock>& drB)
{
	bool closed = false;
	int count = 0;
	dds::core::cond::WaitSet::ConditionSeq conditions;
	do {
		/* wait for topic B */
		conditions = waitsetB.wait();
		std::cout  << "== B call ==" << std::endl;
		for (uint i=0; i < conditions.size(); i++)
		{
			if (conditions[i] != readerSCB) {
				std::cerr << "Something triggered the waitSet" << std::endl;
				continue;
			}
			dds::sub::LoanedSamples<CoherentData::Stock> samplesB = drB.take();
			for (dds::sub::LoanedSamples<CoherentData::Stock>::const_iterator sampleB = samplesB.begin();
				sampleB < samplesB.end();
				++sampleB)
			{
				if (sampleB->info().valid())
				{
					if(sampleB->data().price() < -0.0f)
					{
						closed = true;
						break;
					}
					std::cout  << std::fixed << std::setprecision(1) 
								<< "   " << sampleB->data().ticker() << "   " << sampleB->data().price() 
								<< "   " << sampleB->data().publisher() << "   " << sampleB->data().strength() << std::endl;
				}
			}
			
			std::cout  << "   -- loop B: " << count << " --" << std::endl;
			++count;
		}
	}while (!closed && count < 1500);
}

void usagePub()
{
    std::cerr << "*** ERROR" << std::endl;
    std::cerr << "*** usage : subscriber <coherent_type> " << std::endl;
	std::cerr << "<coherent_type>: 1 instance; 2 topic; 3 group" << std::endl;
}

static int clamp(int v, int v_min, int v_max)
{
    return std::min(std::max(v_min,v),v_max);
}
