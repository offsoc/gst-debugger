/*
 * sigc++lambdahack.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_SIGC__LAMBDAHACK_H_
#define SRC_GST_DEBUGGER_SIGC__LAMBDAHACK_H_

// see here: http://stackoverflow.com/questions/13805883/use-lambda-in-connection-with-sigc-library

#include <type_traits>
#include <sigc++/sigc++.h>
namespace sigc
{
template <typename Functor>
struct functor_trait<Functor, false>
{
	typedef decltype (::sigc::mem_fun (std::declval<Functor&> (),
			&Functor::operator())) _intermediate;

	typedef typename _intermediate::result_type result_type;
	typedef Functor functor_type;
};
}

#endif /* SRC_GST_DEBUGGER_SIGC__LAMBDAHACK_H_ */
