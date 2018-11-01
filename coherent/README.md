In this example, you'll need 4 terminals to test the behavier of the coherent samples.

## Quick note
```shell
# for publisher
./build/coherent/coherent_pub pubC pubD 1 0.5 2 5 2
./build/coherent/coherent_pub pubA pubB 1 0.8 2 10 1
# for subscriber
./build/coherent/coherent_sub 1
./build/coherent/coherent_sub 1
```

## usage:
* Publihser  
    `coherent_pub <pubA_name> <pubB_name> <topic_pub_interval> <sample_pub_interval> <max_iterations> <coherent_choice>`  
    The publihser will be writing 4 samples of data to two topics "_CoherentTopicA_" and "_CoherentTopicB_". These topics are essentially the same in terms of datatype, QoS, and setting.  
    The publisher will first initial coherent set before any data writing. Then, first 2 samples with 0.5 increasing numbers will be written to the dataWriter with time interval of **_<sample_pub_interval>_**.  
    After **_<topic_pub_interval>_** amount of time, 3rd and 4th samples belong to the second topic will be written and the loop will sleep for **_<iter_interval>_** before restart the process again.
    * < pubA_name > : A name string in payload for datawriter A
    * < pubB_name > : A name string in payload for datawriter B
    * < topic_pub_interval > : Time interval between the publish of first topic and second topic
    * < sample_pub_interval > : Time interval between the publish of two samples within the same topic
    * < iter_interval > : Time interval after the wrtten of the final sample and next round of publishes
    * < max_iterations > : How many time the publishing procedure will loop
    * < coherent_choice > : [0] do not activate coherent / [1] presentation QoS: Instance / [2] presentation QoS: Topic / [3] presentation QoS: Group

* Subscriber
    `subscriber <coherent_type>`  
    The subscriber will open two data readers for each of the topic. These two data readers will than be put into two seperate thread and blocked with DDS waitset. Both thread will print the data (using std::cout) within the sample as soon as the waitset is released.
    * < coherent_type >: [1] instance / [2] topic / [3] group

