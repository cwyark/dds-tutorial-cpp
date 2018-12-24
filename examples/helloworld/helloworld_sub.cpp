#include <iostream>
#include <thread>
#include <chrono>
#include <gen/HelloWorld_DCPS.hpp>

int main (int argc, char *argv[]) {
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();

  dds::topic::Topic<HelloWorldData::Msg> topic(dp, "HelloWorldData_Msg", topicQos);

  std::string name = "HelloWorld Example";
  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos() 
                        << dds::core::policy::Partition(name);
  dds::sub::Subscriber sub(dp, subQos);

  dds::sub::qos::DataReaderQos drQos = topic.qos();
  dds::sub::DataReader<HelloWorldData::Msg> dr(sub, topic, drQos);

  bool sampleReceived = false;

  int count = 0;

  do {
    dds::sub::LoanedSamples<HelloWorldData::Msg> samples = dr.take();
    for (dds::sub::LoanedSamples<HelloWorldData::Msg>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << "message received :" << std::endl;
        std::cout << "userID: " << sample->data().userID() << std::endl;
        std::cout << "message: " << sample->data().message() << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count++;
  } while (!sampleReceived && count < 600);

  if (!sampleReceived) {
    std::cout << "ERROR: timeout but no sample received" << std::endl;
    return 1;
  }

  return 0;
}
