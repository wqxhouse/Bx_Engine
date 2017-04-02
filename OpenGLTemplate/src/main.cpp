/*

	@author: Xiangshun Bei(bxs3514)
	@date: 07/20/2016
*/

#include "Context/OpenGLContext.h"

int main()
{
	Setting setting;
	OpenGLContext m_context(setting);
	m_context.run();

	return 0;
}