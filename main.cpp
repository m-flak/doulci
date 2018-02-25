#include "tf-uid.hpp"

#include <iostream>
int main(int argc, char **argv)
{
	doulci::sys::temp_uid process_tfuid;

	try
	{
		process_tfuid.create_file();
	}
	catch (doulci::sys::exception_tfuid& e)
	{
		const int code = e.what_code();
		std::cerr << e.what() << '\n';
		
		if (code == EXCEPTION_TFUID_ALREADY_EXISTS)
		{
			std::cerr << "Another instance of doulci is running for this user id (UID).\n"
					  << "If you know this be in error, execute: "
					  << "\'rm /tmp/doulci.`uid -u`\' in a shell/terminal.\n";
			return 1;
		}
		else if (code == EXCEPTION_TFUID_CREATION)
		{
			std::cerr << "Cannot create our lock/state file!!!\nABORTING...\n";
			return 1;
		}
	}

	return 0;
}

