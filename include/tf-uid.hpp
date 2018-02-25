#ifndef _TF_UID_HPP_
#define _TF_UID_HPP_

#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "doulci.hpp"

/****************** xcept codes ***********************/
#define EXCEPTION_TFUID_UNDEFINED 	-1
#define EXCEPTION_TFUID_CREATION 	1000
#define EXCEPTION_TFUID_ALREADY_EXISTS  2000

// maximum threads, see doulci.hpp
constexpr auto MAX_THREAD = static_cast<int>(doulci::max_cap_threads());

namespace doulci
{
	namespace sys
	{
		// CONTAINER CLASS FOR...
		//  anything that needs to be put inside the file assoc.'d with temp_uid
		template<class A>
		struct tf_content
		{
			typedef typename A::_A type_content;
			
			A *content;
			tf_content() { content = new A(); }
			virtual ~tf_content() { delete content; }
			virtual const A& get_content() { static A x(*content); return x; }

			template <class T>
			void wrout(auto& stream, type_content pcontent, bool newline=true)
			{
				if (!newline)
				{
					stream << static_cast<T>(pcontent);
				}
				stream << static_cast<T>(pcontent) << '\n';
			}
		};
		
		// Thread limit we'll put into the file and f*** wit'
		// ANYTHING NEEDING TO BE USED WITH tf_limit MUST HAVE `_A` typedef
		template<int N, typename A = int>
		struct tf_limit
		{
			/***** REQUIRED FOR tf_content UTILIZATION ******/
			typedef A _A;
			
			static const int limit()
			{
				return N;
			}
		};

		// Exception class for problems occurring with the `temp_uid` class
		class exception_tfuid : public std::exception
		{
		public:
			explicit exception_tfuid(const std::string& msg) :
				p_thewhat(p_initial_msg), p_whatcode(EXCEPTION_TFUID_UNDEFINED)
			{ p_thewhat.append(msg); }

			exception_tfuid(const std::string& msg, const std::string& filename) :
				p_thewhat(p_initial_msg), p_whatcode(EXCEPTION_TFUID_UNDEFINED)
			{
				p_thewhat.append(msg);
				p_thewhat += " with ";
				p_thewhat.append(filename);
			}

			exception_tfuid(const std::string& msg, const std::string& filename, const int ecode) :
				p_thewhat(p_initial_msg), p_whatcode(ecode)
			{
				p_thewhat.append(msg);
				p_thewhat += " with ";
				p_thewhat.append(filename);
			}

			virtual const char* what() const noexcept
			{
				return p_thewhat.c_str();
			}

			virtual const int what_code() const noexcept
			{
				return p_whatcode;
			}
		private:
			constexpr static const char *p_initial_msg = "TEMP_UID Except: ";
			std::string		     p_thewhat;
			int			     p_whatcode;
		};

		// Variable template helpers for `temp_uid`
		template<class T>
		constexpr T _uid = T(0);
		template<class T>
		T _get_uid(void)
		{
			return _uid<T> + getuid();
		}

		// TEMPORARY FILE for like process control
		class temp_uid
		{
		protected:
			/*** XXXXX before the period ***/
			constexpr static const char *first_ingredient = "doulci";
			
			/*** CURRENT USER UID ***/
			static const int 	    second_ingredient(void) 
			{
				return doulci::sys::_get_uid<pid_t>();
			}
		
		public:
			/*** GENERIC CTOR ***/
			temp_uid() : file_name(std::string(first_ingredient)+std::string(".")+std::to_string(second_ingredient())),
				fqn_path({'/','t','m','p','/'}), creator_uid(doulci::sys::_get_uid<pid_t>()), handle_existent(false)
			{
				this->fqn_path.append(this->file_name);
			}
			
			/*** MANUAL-WAY CTOR ***/
			temp_uid(const char *pre_period, int post_period)
			{
				char buffer[32] = {'\0'};
				size_t p1_size = strlen(pre_period);
				snprintf(buffer, 32, "%d", post_period);
				
				file_name = std::string(".");
				file_name.append(buffer);
				file_name.insert(file_name.cbegin(), pre_period, pre_period+p1_size);
				fqn_path = std::string(file_name);
				fqn_path.insert(fqn_path.begin(), {'/','t','m','p','/'});
				creator_uid = post_period;
				handle_existent = false;
			}
			
			virtual ~temp_uid() { /*...*/ }
		
			virtual void create_file()
			{
				int fd = open(fqn_path.c_str(), O_CREAT | O_RDWR | O_TRUNC | O_EXCL, 0664);
				if (fd == -1)
				{
					if (errno == EEXIST)
					{
						throw doulci::sys::exception_tfuid("Already Exists!", this->file_name,
								EXCEPTION_TFUID_ALREADY_EXISTS);
					}

					throw doulci::sys::exception_tfuid("Creation Error!", this->file_name,
							EXCEPTION_TFUID_CREATION);
				}
				close(fd);

				file_handle = std::fstream(fqn_path, std::ios_base::in | std::ios_base::out);

				if (file_handle.fail() || file_handle.bad())
				{
					throw doulci::sys::exception_tfuid("ERROR", this->file_name);
				}

				handle_existent = true;
				
				thread_limit.wrout<int>(file_handle, thread_limit.content->limit());
			}

			std::string fqn_path;
			std::string file_name;
			int creator_uid;
			bool handle_existent;

			tf_content<tf_limit<MAX_THREAD>>	thread_limit; // line 1
		private:
			std::fstream file_handle;
		};
	};
};

#endif

