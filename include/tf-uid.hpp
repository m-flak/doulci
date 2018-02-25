/** 
 *  @file    tf-uid.hpp
 *  @author  Matthew E. Kehrer <mekehre@rdwr.pw>
 * 
 * @brief Implements a TEMPORARY FILE for LOCKING purposes and such what not.
 * 
 * @section Low-Down
 * 
 * Implements the functionality for locking, process spawn control via __user-id lockfiles__
 * __DOULCI__ utilizes threading verses using fork() and friends. We don't want our server to
 * be forked senselessly in the event of malicious attacker.
 */
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

/*@{*/
/****************** xcept codes ***********************/
#define EXCEPTION_TFUID_UNDEFINED 	-1
#define EXCEPTION_TFUID_CREATION 	1000
#define EXCEPTION_TFUID_ALREADY_EXISTS  2000
/*@}*/

/*! maximum threads
 * @see doulci.hpp
 */
constexpr auto MAX_THREAD = static_cast<int>(doulci::max_cap_threads());

/** @namespace doulci
 * Project namespace
 */
namespace doulci
{
	/** @namespace doulci::sys
	 * Implements low-level and/or general components.
	 */
	namespace sys
	{
		/** @struct tf_content
		 *
		 * 
		 * @brief Container class for anything that needs to be put inside the file associated with temp_uid
		 * @tparam A templated class that __must__ have a template def. formed [like so.](@ref tf_limit)
		 * @note You can access content directly or with the accessor. __(keep reading)__
		 * @remarks   class A   __must__ have a member:   _A   .
		 */
		
		template<class A>
		struct tf_content
		{
			/** @typedef type_content
			 *
			 * @brief The type of content in this here container. Compliant classi __must__ have an    _A typdef   member.
			 * @ref tf_limit
			 */
			typedef typename A::_A type_content;
			
			A *content;
			tf_content() { content = new A(); }
			virtual ~tf_content() { delete content; }
			virtual const A& get_content() { static A x(*content); return x; }

			/** __Wr__ __out__ passed data of   type_content  to the   temp_uid   file
			 * @brief output data to temp_uid's   std::fstream  
			 * @tparam T The type that   type_content   shall be casted to
			 * @param stream A reference to a   std::fstream  
			 * @param pcontent data to go into the file
			 * @param newline ...
			 */
			template <class T>
			void wrout(auto& stream, type_content pcontent, bool newline=true)
			{
				if (!newline)
				{
					stream << static_cast<T>(pcontent);
				}
				stream << static_cast<T>(pcontent) << '\n';
			}

			/** __Wr__ __out__ passed data of   type_content  to the   temp_uid   file
			 * @brief output data to temp_uid's   std::fstream  
			 * @tparam T The type that   type_content   shall be casted to
			 * @param stream A reference to a   std::fstream  
			 * @param pcontent data to go into the file
			 * @param newline ...
			 * @param pre_call @ref pre_call_cb
			 * @anchor pre_call_cb
			 * @htmlonly
			  <div style="display: block; position:relative; background-color: #f1f8ff; color: #586069; padding: 4 4 4 4;" id="pre_call_cb">
			  <p> The callback is as follows:</p>
			 <p>
			  		<strong>pre_call(std::fstream& p1, int *offset_file);</strong>
			  		<span class="paramname"><blockquote>
						p1 -- std::fstream<br>
						offset_file -- ptr to offset<br>
					</blockquote>
					</span>
				</p>
			 </div>
			 * @endhtmlonly
			 */
			template <class T>
			void wrout(auto& stream, type_content pcontent, bool newline=true, void(*pre_call)(void*,int*)=nullptr)
			{
				int pozzed = content->seekpos();
				if (pre_call)
					pre_call((void*)&stream, &pozzed);
				
				if (!newline)
				{
					stream << static_cast<T>(pcontent);
				}
				stream << static_cast<T>(pcontent) << '\n';
			}
		};

		/** @struct tf_seekable
		 * @brief Interface class for use with   tf_content  
		 * ITEMS IN CONTAINERS OF ABOVE MUST INHERIT YE BELOW
		 */
		struct tf_seekable
		{
			virtual int seekpos(void) = 0;
		};
		
		/** @struct tf_limit
		 * 
		 * @implements tf_content
		 * 
		 * @remarks ANYTHING NEEDING TO BE USED WITH tf_limit MUST HAVE `_A` typedef
		 * @note Please look carefully at the template def., from rite 2 left.
		 * @tparam N maximum number of threads to have in our pool
		 * @tparam A the typename of template parameter 1.
		 */
		template<int N, typename A = int>
		struct tf_limit : public tf_seekable
		{
			/** @typedef _A
			 *
			 * @see tf_content
			 * @warning REQUIRED by @relates tf_content
			 */
			typedef A _A;
			
			static const int limit()
			{
				return N;
			}
			
			/** Implentor of the interface as per   tf_seekable  
			 * @retval seek_position An integer that is the offset to the   tf_seekable  's location
			 */
			virtual int seekpos(void) { return 3; }
			virtual ~tf_limit() { }
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
				
				thread_limit.wrout<int>(file_handle, thread_limit.content->limit(), true, [](void* p1, int *p2){
					auto *pp1 = reinterpret_cast<decltype(file_handle)*>(p1);
					if (pp1->tellp() == 0)
						if (pp1->tellp() == pp1->tellg())
							return;
				
					if (pp1->tellp() != *p2)
						pp1->seekp(*p2);
					if (pp1->tellg() != *p2)
						pp1->seekg(*p2);
				});
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

