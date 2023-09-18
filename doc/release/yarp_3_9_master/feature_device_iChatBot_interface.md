feature_device_iChatBot_interface {#master}
---------------

### Devices - libYARP_dev

#### `IChatBot`

* Added a `yarp` `interface` to interact with chatbots. It allows to send messages and receive answers and to manage the bot language.

* This does not features "implementation-specific" functionalities that could benefit the user but are not universally available among the various chatbot that can be found online. The idea is to open new PR in the future with new interfaces derived from `yarp::dev::IChatBot` that will model different "classes" of
chatbots (an example on how chatbots can be classified, can be found in this article: [A critical review of state-of-the-art chatbot designs and applications](https://doi.org/10.1002/widm.1434))