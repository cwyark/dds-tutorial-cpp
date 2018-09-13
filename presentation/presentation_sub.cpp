#include <iostream>
#include <thread>
#include <chrono>
#include <gen/PresentationData_DCPS.hpp>

int main (int argc, char *argv[]) {
  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable()
                        << dds::core::policy::History::KeepAll();

  dds::topic::Topic<PresentationData::Msg> topic(dp, "PresentationData_Msg", topicQos);

  std::string name = "Presentation Example";
  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos() 
                        << dds::core::policy::Partition(name)
                        << dds::core::policy::Presentation::TopicAccessScope(false, true);
  dds::sub::Subscriber sub(dp, subQos);

  dds::sub::qos::DataReaderQos drQos = topic.qos();

  dds::sub::DataReader<PresentationData::Msg> dr(sub, topic, drQos);

  bool sampleReceived = false;

  int count = 0;

  do {
    dds::sub::LoanedSamples<PresentationData::Msg> samples = dr.take();
    for (dds::sub::LoanedSamples<PresentationData::Msg>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << "message received :";
        std::cout << "userID: " << sample->data().userID();
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
