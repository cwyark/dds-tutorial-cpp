#include <iostream>
#include <gen/topickey_DCPS.hpp>
#include <random>
#include <thread>
#include <chrono>

void test_keyed_topic(void) {

  // Join or create the defaut domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create the keyed and keyless topic
  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable()
                        << dds::core::policy::History::KeepAll();
  dds::topic::Topic<TopicKey::KeyedTempSensor> keyed_topic(dp, "KeyedTopic", topicQos);

  // Create publisher
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos();
  dds::pub::Publisher pub(dp, pubQos);

  // Create DataWriter
  dds::pub::qos::DataWriterQos dwQos = keyed_topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();
  dds::pub::DataWriter<TopicKey::KeyedTempSensor> keyed_writer(pub, keyed_topic, dwQos);


  // Create Subscriber
  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos();
  dds::sub::Subscriber sub(dp, subQos);

  // Create DataReader
  dds::sub::qos::DataReaderQos drQos = keyed_topic.qos();
  dds::sub::DataReader<TopicKey::KeyedTempSensor> keyed_reader(sub, keyed_topic, drQos);

  std::random_device rd;
  std::default_random_engine gen = std::default_random_engine(rd());
  std::uniform_real_distribution<float> dis(1.0,100.0);
  
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      TopicKey::KeyedTempSensor msgInstance(j, dis(gen));
      keyed_writer << msgInstance;
      std::cout << "Writing message to keyed topic ";
      std::cout << " sensorID " << msgInstance.sensorID();
      std::cout << " value " << msgInstance.value() << std::endl;
    }
  }

  std::cout << "Now start receiving message" << std::endl;
  int count = 3;
  do {
    dds::sub::LoanedSamples<TopicKey::KeyedTempSensor> samples = keyed_reader.take();
    for (dds::sub::LoanedSamples<TopicKey::KeyedTempSensor>::const_iterator sample = samples.begin();
        sample < samples.end(); ++sample) {
          if (sample->info().valid()) {
            std::cout << "message received: ";
            std::cout << " sensorID: " << sample->data().sensorID();
            std::cout << " value: " << sample->data().value() << std::endl;
          }
        }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (count--);
}

void test_keyless_topic(void) {

  // Join or create the defaut domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create the keyed and keyless topic
  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable()
                        << dds::core::policy::History::KeepAll();
  dds::topic::Topic<TopicKey::KeylessTempSensor> keyless_topic(dp, "KeylessTopic", topicQos);

  // Create publisher
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos();
  dds::pub::Publisher pub(dp, pubQos);

  // Create DataWriter
  dds::pub::qos::DataWriterQos dwQos = keyless_topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();
  dds::pub::DataWriter<TopicKey::KeylessTempSensor> keyless_writer(pub, keyless_topic, dwQos);


  // Create Subscriber
  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos();
  dds::sub::Subscriber sub(dp, subQos);

  // Create DataReader
  dds::sub::qos::DataReaderQos drQos = keyless_topic.qos();
  dds::sub::DataReader<TopicKey::KeylessTempSensor> keyless_reader(sub, keyless_topic, drQos);

  std::random_device rd;
  std::default_random_engine gen = std::default_random_engine(rd());
  std::uniform_real_distribution<float> dis(1.0,100.0);
  
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      TopicKey::KeylessTempSensor msgInstance(j, dis(gen));
      keyless_writer << msgInstance;
      std::cout << "Writing message to keyless topic";
      std::cout << " sensorID " << msgInstance.sensorID();
      std::cout << " value " << msgInstance.value() << std::endl;
    }
  }

  std::cout << "Now start receiving message" << std::endl;

  int count = 3;
  do {
    dds::sub::LoanedSamples<TopicKey::KeylessTempSensor> samples = keyless_reader.take();
    for (dds::sub::LoanedSamples<TopicKey::KeylessTempSensor>::const_iterator sample = samples.begin();
        sample < samples.end(); ++sample) {
          if (sample->info().valid()) {
            std::cout << "message received: ";
            std::cout << " sensorID: " << sample->data().sensorID();
            std::cout << " value: " << sample->data().value() << std::endl;
          }
        }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (count--);
}


int main (int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <key type>" << std::endl;
    return 1;
  }
  
  std::string command = argv[1];

  if (command == "keyed")
    test_keyed_topic();
  else if (command == "keyless")
    test_keyless_topic();
  else {
    std::cout << "Unknown command" << std::endl;
    return 1;
  }

  return 0;
}
