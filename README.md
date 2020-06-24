# discretesimulationqueues
The main goal of the present work is learned how to simulate discrete systems related to queues, and do a deep analysis comparing different characteristics in M/M/1 queues .  Some important performance parameters for queues are defined in the present work such as the average number of packets in the system as seen by an arrival or an observer, average number of packets in queue as seen by an arrival or an observer, idle time as seen by an observer, sojourn time, and probability of loss. It is important to highlight that this work implements as an event an observer which comes to the queue in order to estimate the performance metrics of queues in an accurate way. In addition, this work shows a form to simulate queues optimizing the time of execution in C++ working with different methods to sort events in an event scheduler and calculate departure times with an optimize algorithm. The algorithms presented in this work are flexible and it can be changed easily in order to simulate different kind of queues with minimal changes.
