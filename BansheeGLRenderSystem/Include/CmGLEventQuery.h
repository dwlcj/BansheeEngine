#pragma once

#include "CmGLPrerequisites.h"
#include "CmEventQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc EventQuery
	 */
	class BS_RSGL_EXPORT GLEventQuery : public EventQuery
	{
	public:
		GLEventQuery();
		~GLEventQuery();

		/**
		 * @copydoc EventQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc EventQuery::isReady
		 */
		virtual bool isReady() const;

	private:
		GLuint mQueryObj;
	};
}