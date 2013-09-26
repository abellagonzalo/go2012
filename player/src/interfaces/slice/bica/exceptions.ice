#ifndef EXCEPTIONS_ICE
#define EXCEPTIONS_ICE

module bica{

  exception robotException
  {
    //! Error description.
    string what;
  };
  
  //! Server failed to configure itself as requrested by client.
  exception ConfigurationNotExistException extends robotException {};

  /*!
    Raised when the server does not have the requested data.
    
    Typically, this is because the server has not fully initialized yet.
  */
  exception DataNotExistException extends robotException {};
  
  //! Indicates a problem with robot hardware, e.g. sensors and actuators.
  exception HardwareFailedException extends robotException {};

  //! Raised when the server is unable to return a topic for subscription.
  exception NoTopicException extends robotException {};

  //! Raised when the server fails to subscribe client for periodic updates.
  exception SubscriptionFailedException extends robotException {};

  //! Raised when the server fails to push initial data to a new subscriber.
  exception SubscriptionPushFailedException extends robotException {};

}; /*module*/

#endif /*EXCEPTIONS_ICE*/
