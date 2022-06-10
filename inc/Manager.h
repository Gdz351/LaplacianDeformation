#pragma once
#define USER_DEFINED_SHADER
#include <GWindow.h>
#include "Model.h"
#include "Deformer.h"

struct PixelInfo {
	float _object_id = 0.f;
	float _prim_id = 0.f;
	float _vertex_id = 0.f;
	float _other_id = 0.f;
};
enum State {
	Fix, Unconstrain, Handles
};

class Manager:public YRender::GWindow
{
public:
	Manager(std::string title, double width, double height, const std::string& model_path);
protected:
	void mouse_button_callback(GLFWwindow* window, int button, int action, int modifiers) override;
	void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) override;
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)override;
	void framebuffer_size_callback(GLFWwindow* window, int width, int height) override;
private:
	PixelInfo get_click_info(double x, double y);
	void update_framebuffer_size(int width, int height);
	void switch_shader(int type = 0);
private:

	PixelInfo _pixel_info;
	unsigned int _fbo, _texture, _rbo;
	Eigen::VectorXd _state;
	int _m_id;
	std::shared_ptr<Model> _model;
	std::shared_ptr < Deformer> _deformer;

};

