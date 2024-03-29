#ifndef _STC_GL_HOST_HPP_
#define _STC_GL_HOST_HPP_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <epoxy/gl.h>
#include <GLFW/glfw3.h>

#include <boost/optional.hpp>

#include "stc/core/basic_host.hpp"

namespace stc
{
namespace gl
{

class context;

class host : public core::basic_host
{
	public:
	/**
	 * Initialize the host object
	 */
	host();

	/**
	 * Frees up resources used by the host object
	 */
	~host();

	void allocate() override;

	core::image render(const std::string &id, boost::optional<int> frame, size_t width, size_t height,
					   const std::array<float, 4> &mouse, GLenum format) override;

	void reset(const std::string &id) override;

	std::string create_local(const std::vector<std::pair<std::string, std::string>> &bufferSources) override;

	std::shared_ptr<core::basic_context> get_context(const std::string &id) override;

	std::shared_ptr<context> get_gl_context(const std::string &id);

	private:
	/**
	 * Instantiate a new context from the given arguments.
	 */
	template <class... T> inline std::shared_ptr<context> new_context(T &&... args)
	{
		// Get default size
		int width, height;
		glfwGetFramebufferSize(st_window, &width, &height);

		// Allocate context
		auto ptr(std::make_shared<context>(std::forward<T>(args)..., width, height));

		// Add to context map
		st_contexts.insert(std::make_pair(ptr->id(), ptr));

		return ptr;
	}

	/// OpenGL window for rendering
	GLFWwindow *st_window;
	/// Number of allocated local contexts
	int local_counter;
	/// List of rendering contexts by name
	std::map<std::string, std::shared_ptr<context>> st_contexts;

	// Allocation state
	bool m_remoteInit;
	bool m_glfwInit;
};
}
}

#endif /* _STC_GL_HOST_HPP_ */
