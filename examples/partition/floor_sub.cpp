#include <iostream>
#include <thread>
#include <chrono>
#include <gen/BlockData_DCPS.hpp>

int main (int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <partition 1> <partition 2> <partition 3> ..." << std::endl;
    return 1;
  }

  std::vector<std::string> arguments(argv, argv + argc);

  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();

  dds::topic::Topic<BlockData::Msg> topic(dp, "BlockData_Msg", topicQos);

  dds::core::StringSeq names = arguments;
  // Remove the first element
  names.erase(names.begin());

  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos() 
                        << dds::core::policy::Partition(names);
  dds::sub::Subscriber sub(dp, subQos);

  dds::sub::qos::DataReaderQos drQos = topic.qos();
  dds::sub::DataReader<BlockData::Msg> dr(sub, topic, drQos);

  for (std::string name : names)
    std::cout << name << std::endl;

  bool sampleReceived = false;
  int count = 0;

  do {
    dds::sub::LoanedSamples<BlockData::Msg> samples = dr.take();
    for (dds::sub::LoanedSamples<BlockData::Msg>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << "message received :" << std::endl;
        std::cout << "blockID: " << sample->data().blockID()<< std::endl;
        std::cout << "value: " << sample->data().value() << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count++;
  } while (!sampleReceived && count < 60000);

  if (!sampleReceived) {
    std::cout << "ERROR: timeout but no sample received" << std::endl;
    return 1;
  }

  return 0;
}
