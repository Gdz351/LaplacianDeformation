#include "Manager.h"
#include <EGLM.h>
#include <iostream>

Manager::Manager(std::string title, double width, double height, const std::string& model_path)
	:GWindow(title, width, height) 
{
	update_framebuffer_size(width, height);
	_model = std::make_shared<Model>(model_path);

	_deformer = std::make_shared<Deformer>(_model->V, _model->F);
	_m_id = push_object(_model->V, _model->F, YRender::Y_TRIANGLES);

	switch_shader(0);
	set_object_polygon_mode(_m_id, YRender::Y_LINE);
}

void Manager::mouse_button_callback(GLFWwindow* window, int button, int action, int modifiers) {
	GWindow::mouse_button_callback(window, button, action, modifiers);
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if (action == GLFW_PRESS) {
		PixelInfo info =get_click_info(x, y);
		float area_id = info._other_id;
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			auto face = _model->_mesh.faces_begin() + (info._prim_id - 1);
			float dist = DOUBLE_INF;
			int src_id = -1;
			for (auto fv : _model->_mesh.fv_range(*face)) {
				if (fabs(info._vertex_id - 1 - fv.idx() < dist)) {
					dist = info._vertex_id - 1 - fv.idx();
					src_id = fv.idx();
				}
			}
			_model->update_vertex_state(src_id, 0.3, 0.4);
			_deformer->update_matrix(_model->_handles_id, _model->_fix_id);
			_state.resize(_model->V.cols());
			for (int f_id : _model->_fix_id)_state(f_id) = Fix;
			for (int h_id : _model->_handles_id) _state(h_id) = Handles;
			for (int u_id : _model->_unconstrain_id) _state(u_id) = Unconstrain;
			update_object(_m_id, _model->V, _model->F, YRender::Y_TRIANGLES,false, {}, { _state });
		}
	}
}
void Manager::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {

	float area_id = _pixel_info._other_id;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		if (area_id < 2.6) {
			GWindow::cursor_pos_callback(window, xpos, ypos);
			set_shader_attri("model", get_trans_mat4());
		}
		else {
			glm::mat4 i_rotate = glm::inverse(_rotate);
			glm::vec3 right = i_rotate * glm::vec4(1., 0., 0., 1.0);
			glm::vec3  up = i_rotate * glm::vec4(0., 1., 0., 1.0);

			if (_left_first) {
				_left_x = xpos; _left_y = ypos;
				_left_first = false;
			}
			float xoffset = xpos - _left_x, yoffset = _left_y - ypos;
			_left_x = xpos; _left_y = ypos;
			xoffset *= _mouse_speed;
			yoffset *= _mouse_speed;
			glm::mat4 trans(1.0f);
			trans = glm::translate(trans, right * (float)xoffset + up * (float)yoffset);

			_deformer->laplace_deformation(_model->V, GLM2E<double, 4, 4>(trans),
				_model->_handles_id, _model->_unconstrain_id, _model->_fix_id);
			update_object(_m_id, _model->V, _model->F, YRender::Y_TRIANGLES, false, {}, { _state });
		}
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
		if (area_id < 2.6) {
			GWindow::cursor_pos_callback(window, xpos, ypos);
			set_shader_attri("model", get_trans_mat4());
		}
		else {
			glm::mat4 i_rotate = glm::inverse(_rotate);
			glm::vec3 right = i_rotate * glm::vec4(1., 0., 0., 1.0);
			glm::vec3  up = i_rotate * glm::vec4(0., 1., 0., 1.0);
			if (_right_first) {
				_right_x = xpos; _right_y = ypos;
				_right_first = false;
			}
			float xoffset = xpos - _right_x, yoffset = _right_y - ypos;
			_right_x = xpos; _right_y = ypos;
			xoffset *= _mouse_speed;
			yoffset *= _mouse_speed;

			glm::mat4 trans(1.0f);
			trans = glm::rotate(trans, (float)xoffset, up);
			trans = glm::rotate(trans, (float)yoffset, -right);

			_deformer->laplace_deformation(_model->V, GLM2E<double, 4, 4>(trans),
				_model->_handles_id, _model->_unconstrain_id, _model->_fix_id);
			update_object(_m_id, _model->V, _model->F, YRender::Y_TRIANGLES, false, {}, { _state });
		}
	}
}

void Manager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	GWindow::framebuffer_size_callback(window, width, height);
	update_framebuffer_size(width, height);
}

void Manager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	GWindow::scroll_callback(window, xoffset, yoffset);
	set_shader_attri("model", get_trans_mat4());
}

PixelInfo Manager::get_click_info(double x, double y)
{
	set_object_polygon_mode(_m_id, YRender::Y_FILL);
	switch_shader(1);	
	refresh(_fbo, true);
	switch_shader(0);
	set_object_polygon_mode(_m_id, YRender::Y_LINE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	PixelInfo pixel_info;
	glReadPixels(x, _height - y, 1, 1, GL_RGBA, GL_FLOAT, &pixel_info);

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	std::cout << pixel_info._object_id << " " << pixel_info._prim_id << " " << pixel_info._vertex_id << " " << pixel_info._other_id << std::endl;
	_pixel_info = pixel_info;
	return pixel_info;
}

void Manager::update_framebuffer_size(int width, int height)
{

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	glGenRenderbuffers(1, &_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error:Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Manager::switch_shader(int type) {

	YRender::Camera _camera = YRender::Camera(glm::vec3(0.0f, 0.0f, 2.0f), false);
	YRender::Lighting _lighting = YRender::Lighting(glm::vec3(-2.2f, -1.0f, -2.3f));
	glm::mat4 projection = glm::perspective<float>(glm::radians(_camera._zoom), (float)_width / _height, 0.1f, 100.0f);

	if (type == 0) {
		set_shader("../shader/main.vs", "../shader/main.fs");
		set_shader_attri("viewPos", GLM2E<double, 3>(_camera._position));
	}
	else {
		set_shader("../shader/interact.vs", "../shader/interact.fs");
		set_shader_attri("gObjectID", 0);
	}
	set_shader_attri("view", GLM2E<double, 4, 4>(_camera.look_at()));
	set_shader_attri("projection", GLM2E<double, 4, 4>(projection));
	set_shader_attri("model", get_trans_mat4());
}