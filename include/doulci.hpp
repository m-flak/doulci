#ifndef _DOULCI_HPP_
#define _DOULCI_HPP_

#include <uWS/uWS.h>
#include <atomic>

namespace doulci {

enum class configuration
{
	/* lol what a hacky way lol*/
	Threads = 16,
	/* so keep all else above 2^4 mkay */
	NonVerbose = 32,
};
typedef doulci::configuration Configurations;
constexpr Configurations max_cap_threads()
{
	return doulci::configuration::Threads;
}

//
// OUR MAIN CLASS
// // xfer of program flow is xfer'd over to here
//

class adoulci
{
public:
	adoulci() :
		thread_counter(0), p_progconf(0)
	{ /***/ }
	explicit adoulci(Configurations opts);
	virtual ~adoulci();

	std::atomic<int> thread_counter;
private:
	// configuration flags, it == 16 || 0, means default config yo
	unsigned int p_progconf;
};

};

#endif
