#define _CRT_SECURE_NO_WARNINGS

#include "App.hpp"

#include "utility.hpp"
#include "base/Main.hpp"
#include "gpu/GLContext.hpp"
#include "gpu/Buffer.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <type_traits>

using namespace FW;
using namespace std;

// Anonymous namespace. This is a C++ way to define things which
// do not need to be visible outside this file.
namespace {

enum VertexShaderAttributeLocations {
	ATTRIB_POSITION = 0,
	ATTRIB_NORMAL = 1,
	ATTRIB_COLOR = 2
};

const Vertex reference_plane_data[] = {
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1, -1), Vec3f(0, 1, 0) },
	{ Vec3f( 1, -1,  1), Vec3f(0, 1, 0) },
	{ Vec3f(-1, -1,  1), Vec3f(0, 1, 0) }
};

vector<Vertex> loadExampleModel() {
	static const Vertex example_data[] = {
		{ Vec3f( 0.0f,  0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f(-0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) },
		{ Vec3f( 0.5f, -0.5f, 0), Vec3f(0.0f, 0.0f, -1.0f) }
	};
	vector<Vertex> vertices;
	for (auto v : example_data)
		vertices.push_back(v);
	return vertices;
}

vector<Vertex> unpackIndexedData(
	const vector<Vec3f>& positions,
	const vector<Vec3f>& normals,
	const vector<array<unsigned, 6>>& faces)
{
	vector<Vertex> vertices;
	Vertex v0, v1, v2;

	// This is a 'range-for' loop which goes through all objects in the container 'faces'.
	// '&' gives us a reference to the object inside the container; if we omitted '&',
	// 'f' would be a copy of the object instead.
	// The compiler already knows the type of objects inside the container, so we can
	// just write 'auto' instead of having to spell out 'array<unsigned, 6>'.
	for (auto& f : faces) {

		// YOUR CODE HERE (R3)
		// Unpack the indexed data into a vertex array. For every face, you have to
		// create three vertices and add them to the vector 'vertices'.

		// f[0] is the index of the position of the first vertex
		// f[1] is the index of the normal of the first vertex
		// f[2] is the index of the position of the second vertex
		// ...
		v0.position = positions[f[0]]; v1.position = positions[f[2]]; v2.position = positions[f[4]];
		v0.normal = normals[f[1]]; v1.normal = normals[f[3]]; v2.normal = normals[f[5]];
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
	}

	return vertices;
};

// This is for testing your unpackIndexedData implementation.
// You should get a tetrahedron like in example.exe.
vector<Vertex> loadIndexedDataModel() {
	static const Vec3f point_data[] = {
		Vec3f(0.0f, 0.407f, 0.0f),
		Vec3f(0.0f, -0.3f, -0.5f),
		Vec3f(0.433f, -0.3f, 0.25f),
		Vec3f(-0.433f, -0.3f, 0.25f),
	};
	static const Vec3f normal_data[] = {
		Vec3f(0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, 0.3334f, 0.9428f),
		Vec3f(-0.8165f, 0.3334f, -0.4714f),
		Vec3f(0.0f, -1.0f, 0.0f)
	};
	static const unsigned face_data[][6] = {
		{0, 0, 1, 0, 2, 0},
		{0, 2, 3, 2, 1, 2},
		{0, 1, 2, 1, 3, 1},
		{1, 3, 3, 3, 2, 3}
	};
	vector<Vec3f> points(point_data, point_data + SIZEOF_ARRAY(point_data));
	vector<Vec3f> normals(normal_data, normal_data + SIZEOF_ARRAY(normal_data));
	vector<array<unsigned, 6>> faces;
	for (auto arr : face_data) {
		array<unsigned, 6> f;
		copy(arr, arr+6, f.begin());
		faces.push_back(f);
	}
	return unpackIndexedData(points, normals, faces);
}

// Generate an upright cone with tip at (0, 0, 0), a radius of 0.25 and a height of 1.0.
// You can leave the base of the cone open, like it is in example.exe.
vector<Vertex> loadUserGeneratedModel() {
	static const float radius = 0.25f;
	static const float height = 1.0f;
	static const unsigned faces = 40;
	static const float angle_increment = 2 * FW_PI / faces;

	// Empty array of Vertex structs; every three vertices = one triangle
	vector<Vertex> vertices;
	
	Vertex v0, v1, v2;

	// Generate one face at a time
	for(auto i = 0u; i < faces; ++i)	{
		// YOUR CODE HERE (R2)
		// Figure out the correct positions of the three vertices of this face.
		// v0.position = ...
		// Calculate the normal of the face from the positions and use it for all vertices.
		// v0.normal = v1.normal = v2.normal = ...;
		//
		// Some hints:
		// - Try just making a triangle in fixed coordinates at first.
		// - "FW::cos(angle_increment * i) * radius" gives you the X-coordinate
		//    of the ith vertex at the base of the cone. Z-coordinate is very similar.
		// - For the normal calculation, you'll want to use the cross() function for
		//   cross product, and Vec3f's .normalized() or .normalize() methods.

		v0.position = Vec3f(0.0f, 0.0f, 0.0f);
		v1.position = Vec3f(FW::cos(angle_increment * i) * radius, -1.0f, FW::sin(angle_increment * i) * radius);
		v2.position = Vec3f(FW::cos(angle_increment * ((i + 1) % faces)) * radius, -1.0f, FW::sin(angle_increment * ((i + 1) % faces)) * radius);
		v0.normal = v1.normal = v2.normal = cross(v2.position - v0.position, v1.position - v0.position).normalized();
		// Then we add the vertices to the array.
		// .push_back() grows the size of the vector by one, copies its argument,
		// and places the copy at the back of the vector.
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
		v0.position = Vec3f(0.0f, -1.0f, 0.0f);
		v1.position = Vec3f(FW::cos(angle_increment * i) * radius, -1.0f, FW::sin(angle_increment * i) * radius);
		v2.position = Vec3f(FW::cos(angle_increment * ((i + 1) % faces)) * radius, -1.0f, FW::sin(angle_increment * ((i + 1) % faces)) * radius);
		v0.normal = v1.normal = v2.normal = (cross((v2.position - v0.position), (v1.position - v0.position)).normalized());
		vertices.push_back(v0); vertices.push_back(v1); vertices.push_back(v2);
	}
	return vertices;
}

}

App::App(void)
:   common_ctrl_			(CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5),
	current_model_			(MODEL_EXAMPLE),
	model_changed_			(true),
	shading_toggle_			(false),
	shading_mode_changed_	(false),
	camera_rotation_angle_	(0.0f),
	camera_z_angle_			(0.0f),
	translation_			(Vec3f(0.0f,0.0f,0.0f)),
	rotation_y_				(0.0f),
	scaling_x_				(1.0f),
	dragging_				(false),
	animating_				(false)
{
	static_assert(is_standard_layout<Vertex>::value, "struct Vertex must be standard layout to use offsetof");
	initRendering();
	
	common_ctrl_.showFPS(true);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_EXAMPLE,			FW_KEY_1, "Triangle (1)",				&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_USER_GENERATED,		FW_KEY_2, "Generated cone (2)",			&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_INDEXED_DATA,	FW_KEY_3, "Unpacked tetrahedron (3)",	&model_changed_);
	common_ctrl_.addToggle((S32*)&current_model_, MODEL_FROM_FILE,			FW_KEY_4, "Model loaded from file (4)",	&model_changed_);
	common_ctrl_.addSeparator();
	common_ctrl_.addToggle(&shading_toggle_,								FW_KEY_T, "Toggle shading mode (T)",	&shading_mode_changed_);

	window_.setTitle("Assignment 1");

	window_.addListener(this);
	window_.addListener(&common_ctrl_);

	window_.setSize( Vec2i(800, 800) );
}

bool App::handleEvent(const Window::Event& ev) {
	if(animating_) {
		camera_rotation_angle_ -= timer_.end();
	}
	if (model_changed_)	{
		model_changed_ = false;
		switch (current_model_)
		{
		case MODEL_EXAMPLE:
			vertices_ = loadExampleModel();
			break;
		case MODEL_FROM_INDEXED_DATA:
			vertices_ = loadIndexedDataModel();
			break;
		case MODEL_USER_GENERATED:
			vertices_ = loadUserGeneratedModel();
			break;
		case MODEL_FROM_FILE:
			{
				auto filename = window_.showFileLoadDialog("Load new mesh");
				if (filename.getLength()) {
					vertices_ = loadObjFileModel(filename.getPtr());
				} else {
					current_model_ = MODEL_EXAMPLE;
					model_changed_ = true;
				}
			}
			break;
		default:
			assert(false && "invalid model type");
		}
		// Load the vertex buffer to GPU.
		glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	if (shading_mode_changed_) {
		common_ctrl_.message(shading_toggle_ ?
			"Directional light shading using vertex normals; direction to light (0.5, 0.5, -0.6)" :
			"High visibility shading, color from vertex ID");
		shading_mode_changed_ = false;
	}


	if (ev.type == Window::EventType_KeyDown) {
		// YOUR CODE HERE (R1)
		// React to user input and move the model.
		// Look in framework/gui/Keys.hpp for more key codes.
		// Visual Studio tip: you can right-click an identifier like FW_KEY_HOME
		// and "Go to definition" to jump directly to where the identifier is defined.
		if (ev.key == FW_KEY_HOME)
			camera_rotation_angle_ -= 0.05 * FW_PI;
		else if (ev.key == FW_KEY_END)
			camera_rotation_angle_ += 0.05 * FW_PI;
		else if (ev.key == FW_KEY_UP)
			translation_[1] += 0.05;
		else if (ev.key == FW_KEY_DOWN)
			translation_[1] -= 0.05;
		else if (ev.key == FW_KEY_LEFT)
			translation_[0] -= 0.05;
		else if (ev.key == FW_KEY_RIGHT)
			translation_[0] += 0.05;
		else if (ev.key == FW_KEY_W)
			translation_[2] += 0.05;
		else if (ev.key == FW_KEY_S)
			translation_[2] -= 0.05;
		else if (ev.key == FW_KEY_R) {
			if (!animating_) {
				animating_ = 1 - animating_;
				timer_.start();
			}
			else {
				animating_ = 1 - animating_;
				timer_.unstart();
			}
				
		}
		else if (ev.key == FW_KEY_Q)
			scaling_x_ -= 0.05;
		else if (ev.key == FW_KEY_E)
			scaling_x_ += 0.05;
		else if (ev.key == FW_KEY_A)
			rotation_y_ += (2 * FW_PI) / 360;
		else if (ev.key == FW_KEY_D)
			rotation_y_ -= (2 * FW_PI)/ 360;
	}
	
	if (ev.type == Window::EventType_KeyUp) {
	}

	if (ev.type == Window::EventType_Mouse) {
		if (dragging_) {
			camera_rotation_angle_ += ev.mouseDelta[0] * 0.005;
			camera_z_angle_ += ev.mouseDelta[1] * 0.005;

		}
		// EXTRA: you can put your mouse controls here.
		// Event::mouseDelta gives the distance the mouse has moved.
		// Event::mouseDragging tells whether some mouse buttons are currently down.
		// If you want to know which ones, you have to keep track of the button down/up events
		// (e.g. FW_KEY_MOUSE_LEFT).
		if (ev.mouseDragging) {
			if (!dragging_) {
				dragging_ = true;
				origin_drag_ = Vec3f();
			}
		}
		else {
			dragging_ = false;
		}
	}

	if (ev.type == Window::EventType_Close) {
		window_.showModalMessage("Exiting...");
		delete this;
		return true;
	}


	window_.setVisible(true);
	if (ev.type == Window::EventType_Paint)
		render();

	window_.repaint();

	return false;
}

void App::initRendering() {
	// Ask the Nvidia framework for the GLContext object associated with the window.
	// As a side effect, this initializes the OpenGL context and lets us call GL functions.
	auto ctx = window_.getGL();
	
	// Create vertex attribute objects and buffers for vertex data.
	glGenVertexArrays(1, &gl_.static_vao);
	glGenVertexArrays(1, &gl_.dynamic_vao);
	glGenBuffers(1, &gl_.static_vertex_buffer);
	glGenBuffers(1, &gl_.dynamic_vertex_buffer);
	
	// Set up vertex attribute object for static data.
	glBindVertexArray(gl_.static_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.static_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));

	// Load the static data to the GPU; needs to be done only once.
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * SIZEOF_ARRAY(reference_plane_data), reference_plane_data, GL_STATIC_DRAW);
	
	// Set up vertex attribute object for dynamic data. We'll load the actual data later, whenever the model changes.
	glBindVertexArray(gl_.dynamic_vao);
	glBindBuffer(GL_ARRAY_BUFFER, gl_.dynamic_vertex_buffer);
	glEnableVertexAttribArray(ATTRIB_POSITION);
	glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) 0);
	glEnableVertexAttribArray(ATTRIB_NORMAL);
	glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*) offsetof(Vertex, normal));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	// Compile and link the shader program.
	// We use the Nvidia FW for creating the program; it's not difficult to do manually,
	// but takes about half a page of OpenGL boilerplate code.
	// This shader program will be used to draw everything except the user interface.
	// It consists of one vertex shader and one fragment shader.
	auto shader_program = new GLContext::Program(
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		layout(location = 0) in vec4 aPosition;
		layout(location = 1) in vec3 aNormal;
		
		out vec4 vColor;
		
		uniform mat4 uModelToWorld;
		uniform mat4 uWorldToClip;
		uniform float uShading;
		
		const vec3 distinctColors[6] = vec3[6](
			vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 1, 1),
			vec3(1, 0, 0), vec3(1, 0, 1), vec3(1, 1, 0));
		const vec3 directionToLight = normalize(vec3(0.5, 0.5, -0.6));
		
		void main()
		{
			// EXTRA: oops, someone forgot to transform normals here...
			mat4 uModelToWorld_inverse = inverse(uModelToWorld);
			mat4 uModelToWorld_T = transpose(uModelToWorld_inverse);
			vec4 aNormal_transformed = uModelToWorld_T * vec4(aNormal, 1.0);
			vec3 aNormal_xyz = aNormal_transformed.xyz;
			aNormal_xyz = normalize(aNormal_xyz);
			float clampedCosine = clamp(dot(aNormal_xyz, directionToLight), 0.0, 1.0);
			vec3 litColor = vec3(clampedCosine);
			vec3 generatedColor = distinctColors[gl_VertexID % 6];
			// gl_Position is a built-in output variable that marks the final position
			// of the vertex in clip space. Vertex shaders must write in it.
			gl_Position = uWorldToClip * uModelToWorld * aPosition;
			vColor = vec4(mix(generatedColor, litColor, uShading), 1);
		}
		),
		"#version 330\n"
		FW_GL_SHADER_SOURCE(
		in vec4 vColor;
		out vec4 fColor;
		void main()
		{
			fColor = vColor;
		}
		));
	// Tell the FW about the program so it gets properly destroyed at exit.
	ctx->setProgram("shaders", shader_program);

	// Get the IDs of the shader program and its uniform input locations from OpenGL.
	gl_.shader_program = shader_program->getHandle();
	gl_.world_to_clip_uniform = glGetUniformLocation(gl_.shader_program, "uWorldToClip");
	gl_.model_to_world_uniform = glGetUniformLocation(gl_.shader_program, "uModelToWorld");
	gl_.shading_toggle_uniform = glGetUniformLocation(gl_.shader_program, "uShading");
}

void App::render() {
	// Clear screen.
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Enable depth testing.
	glEnable(GL_DEPTH_TEST);
	
	// Set up a matrix to transform from world space to clip space.
	// Clip space is a [-1, 1]^3 space where OpenGL expects things to be
	// when it starts drawing them.

	// Our camera is aimed at origin, and orbits around origin at fixed distance.
	static const float camera_distance = 2.1f;	
	Mat4f C;
	Mat3f rot = Mat3f::rotation(Vec3f(0, 1, 0), -camera_rotation_angle_);
	Mat3f rot2 = Mat3f::rotation(Vec3f(1, 0,0), -camera_z_angle_);
	Mat3f rot3 = rot * rot2;
	C.setCol(0, Vec4f(rot3.getCol(0), 0));
	C.setCol(1, Vec4f(rot3.getCol(1), 0));
	C.setCol(2, Vec4f(rot3.getCol(2), 0));
	C.setCol(3, Vec4f(0, 0, camera_distance, 1));
	
	// Simple perspective.
	static const float fnear = 0.1f, ffar = 4.0f;
	Mat4f P;
	P.setCol(0, Vec4f(1, 0, 0, 0));
	P.setCol(1, Vec4f(0, 1, 0, 0));
	P.setCol(2, Vec4f(0, 0, (ffar+fnear)/(ffar-fnear), 1));
	P.setCol(3, Vec4f(0, 0, -2*ffar*fnear/(ffar-fnear), 0));

	Mat4f world_to_clip = P * C;
	
	// Set active shader program.
	glUseProgram(gl_.shader_program);
	glUniform1f(gl_.shading_toggle_uniform, shading_toggle_ ? 1.0f : 0.0f);
	glUniformMatrix4fv(gl_.world_to_clip_uniform, 1, GL_FALSE, world_to_clip.getPtr());

	// Draw the reference plane. It is already in world coordinates.
	auto identity = Mat4f();
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, identity.getPtr());
	glBindVertexArray(gl_.static_vao);
	glDrawArrays(GL_TRIANGLES, 0, SIZEOF_ARRAY(reference_plane_data));
	
	// YOUR CODE HERE (R1)
	// Set the model space -> world space transform to translate the model according to user input.
	Mat4f modelToWorld = Mat4f();
	Mat4f T, R, S;
	T.setCol(3, Vec4f(translation_, 1.0f));
	R.setRow(0, Vec4f(FW::cos(rotation_y_), 0.0f, FW::sin(rotation_y_), 0.0f));
	R.setRow(2, Vec4f(-FW::sin(rotation_y_), 0.0f, FW::cos(rotation_y_), 0.0f));
	S.m00 = scaling_x_;

	modelToWorld = S * modelToWorld;
	modelToWorld = R * modelToWorld;
	modelToWorld = T * modelToWorld;
	
	// Draw the model with your model-to-world transformation.
	glUniformMatrix4fv(gl_.model_to_world_uniform, 1, GL_FALSE, modelToWorld.getPtr());
	glBindVertexArray(gl_.dynamic_vao);
	glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

	// Undo our bindings.
	glBindVertexArray(0);
	glUseProgram(0);
	
	// Check for OpenGL errors.
	GLContext::checkErrors();
	
	// Show status messages. You may find it useful to show some debug information in a message.
	common_ctrl_.message(sprintf("Use Home/End to rotate camera."), "instructions");
	common_ctrl_.message(sprintf("Camera is at (%.2f %.2f %.2f) looking towards origin.",
		-FW::sin(camera_rotation_angle_) * camera_distance, 0.0f,
		-FW::cos(camera_rotation_angle_) * camera_distance), "camerainfo");
}

vector<Vertex> App::loadObjFileModel(string filename) {
	window_.showModalMessage(sprintf("Loading mesh from '%s'...", filename.c_str()));

	vector<Vec3f> positions, normals;
	vector<array<unsigned, 6>> faces;

	// Open input file stream for reading.
	ifstream input(filename, ios::in);
	string line;
	int face_number = 0, vertex_number = 0, normal_number = 0;
	string word;

	// FORMAT: 0 AASCI - 1 BIG ENDIAN - 2 LITTLE ENDIAN
	int format = 0;

	if (filename.compare(filename.size() - 3, 3, "obj") == 0) {
		// Read the file line by line.

		while (getline(input, line)) {
			// Replace any '/' characters with spaces ' ' so that all of the
			// values we wish to read are separated with whitespace.
			for (auto& c : line)
				if (c == '/')
					c = ' ';

			// Temporary objects to read data into.
			array<unsigned, 6>  f; // Face index array
			Vec3f               v;
			string              s;

			// Create a stream from the string to pick out one value at a time.
			istringstream        iss(line);

			// Read the first token from the line into string 's'.
			// It identifies the type of object (vertex or normal or ...)
			iss >> s;

			if (s == "v") { // vertex position
				// YOUR CODE HERE (R4)
				float a, b, c;
				iss >> a;
				iss >> b;
				iss >> c;
				// Read the three vertex coordinates (x, y, z) into 'v'.
				v = Vec3f(a, b, c);
				positions.push_back(v);
				// Store a copy of 'v' in 'positions'.
				// See std::vector documentation for push_back.
			}
			else if (s == "vn") { // normal
			 // YOUR CODE HERE (R4)
			 // Similar to above.
				float a, b, c;
				iss >> a;
				iss >> b;
				iss >> c;
				v = Vec3f(a, b, c);
				normals.push_back(v);
			}
			else if (s == "f") { // face
			 // YOUR CODE HERE (R4)
			 // Read the indices representing a face and store it in 'faces'.
			 // The data in the file is in the format
			 // f v1/vt1/vn1 v2/vt2/vn2 ...
			 // where vi = vertex index, vti = texture index, vni = normal index.
			 //
			 // Remember we already replaced the '/' characters with whitespaces.
			 //
			 // Since we are not using textures in this exercise, you can ignore
			 // the texture indices by reading them into a temporary variable.
				unsigned sink; // Temporary variable for reading the unused texture indices.
				unsigned temp;
				int discarded = 0;
				for (int i = 0; i < 9; i++) {
					if (i % 3 == 1) {
						iss >> sink;
						discarded++;
					}
					else {
						iss >> temp;
						f[i - discarded] = temp - 1;
					}
				}
				faces.push_back(f);

				// Note that in C++ we index things starting from 0, but face indices in OBJ format start from 1.
				// If you don't adjust for that, you'll index past the range of your vectors and get a crash.

				// It might be a good idea to print the indices to see that they were read correctly.
				// cout << f[0] << " " << f[1] << " " << f[2] << " " << f[3] << " " << f[4] << " " << f[5] << endl;
			}
		}
	}
	// PLY files
	else {
		// Temporary objects to read data into.
		array<unsigned, 6>  f; // Face index array
		Vec3f               v;
		string              s;

		while (getline(input, line)) {
			istringstream iss(line);
			iss >> word;
			if (word.compare("end_header") == 0)
				break;
			if (word.compare("element") == 0) {
				iss >> word;
				if (word.compare("vertex") == 0) {
					iss >> vertex_number;
				}
				else if (word.compare("face") == 0) {
					iss >> face_number;
				}
			}
			else if (word.compare("format") == 0) {
				iss >> word;
				if (word.compare("ascii") == 0) {
					format = 0;
				}
				else if (word.compare("binary_big_endian") == 0) {
					format = 1;
				}
				else if (word.compare("binary_little_endian") == 0) {
					format = 2;
				}
			}
		}

		// TODO: ADD BINARY SUPPORT AND POLYGONS

		// VERTEX READING
		float a, b, c, sink;
		for (int i = 0; i < vertex_number; i++) {
			getline(input, line);
			istringstream iss(line);
			iss >> a >> b >> c;
			v = Vec3f(a, b, c);
			positions.push_back(v);
		}
		// FACES READING
		for (int i = 0; i < face_number; i++) {
			getline(input, line);
			istringstream iss(line);

			iss >> sink;
			vector<int> temp;
			std::cout << "Face n. " << i << endl;
			std::cout << sink << " vertices" << endl;
			for (int j = 0; j < sink; j++) {
				iss >> a;
				std::cout << a << " ";
				temp.push_back(a);
			}
			std::cout << endl << endl;

			// EAR CLIPPING ALGORITHM
			while (temp.size() >= 3) {

				int index = 0;
				bool not_found = true;
				while (not_found) {

					// SEE IF VERTEX IS CONVEX
					Vec3f e1, e2;
					int pos_a, pos_b, pos_c, ind_a, ind_b, ind_c;
					pos_b = temp[index];
					ind_b = index;
					if (index == 0) {
						pos_a = temp[temp.size() - 1];
						ind_a = temp.size() - 1;
					}
					else {
						pos_a = temp[index - 1];
						ind_a = index - 1;
					}
					if (index == temp.size() - 1) {
						pos_c = temp[0];
						ind_c = 0;
					}
					else {
						pos_c = temp[index + 1];
						ind_c = index + 1;
					}

					std::cout << "Index: " << pos_a << "-" << pos_b << "-" << pos_c << endl;

					e1 = Vec3f(positions[pos_a][0] - positions[pos_b][0],
						positions[pos_a][1] - positions[pos_b][1],
						positions[pos_a][2] - positions[pos_b][2]);

					e2 = Vec3f(positions[pos_c][0] - positions[pos_b][0],
						positions[pos_c][1] - positions[pos_b][1],
						positions[pos_c][2] - positions[pos_b][2]);

					float e1mag = sqrt(e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2]);
					float e2mag = sqrt(e2[0] * e2[0] + e2[1] * e2[1] + e2[2] * e2[2]);

					Vec3f e1norm = Vec3f(e1[0] / e1mag, e1[1] / e1mag, e1[2] / e1mag);
					Vec3f e2norm = Vec3f(e2[0] / e2mag, e2[1] / e2mag, e2[2] / e2mag);

					float angle = acos(e1norm[0] * e2norm[0] + e1norm[1] * e2norm[1] + e1norm[2] * e2norm[2]);
					// STOP IF ANGLE IS CONCAVE
					std::cout << "Angle: " << angle << endl;
					if (angle >= FW_PI) {
						std::cout << "Concave: next!" << endl;
						index += 1;
						continue;
					}


					// NOW WE NEED TO CHECK IF NO OTHER VERTEX IS CONTAINED INSIDE THIS TRIANGLE

					e1 = Vec3f(positions[pos_b][0] - positions[pos_a][0],
						positions[pos_b][1] - positions[pos_a][1],
						positions[pos_b][2] - positions[pos_a][2]);

					e2 = Vec3f(positions[pos_b][0] - positions[pos_c][0],
						positions[pos_b][1] - positions[pos_c][1],
						positions[pos_b][2] - positions[pos_c][2]);

					Mat3f T;
					T.setCol(0, positions[pos_a]);
					T.setCol(1, positions[pos_b]);
					T.setCol(2, positions[pos_c]);
					T.invert(); T.transpose();
					std::cout << "T matrix" << endl;
					std::cout << T(0, 0) << "\t" << T(0, 1) << "\t" << T(0, 2) << endl;
					std::cout << T(1, 0) << "\t" << T(1, 1) << "\t" << T(1, 2) << endl;
					std::cout << T(2, 0) << "\t" << T(2, 1) << "\t" << T(2, 2) << endl;
					std::cout << "Inside triangle?" << endl;

					bool no_points_inside = true;
					for (int k = 0; k < temp.size(); k++) {
						if (k == ind_a || k == ind_b || k == ind_c) {
							std::cout << k << " is a triangle vertex." << endl;
							continue;
						}
						Vec3f P = positions[temp[k]];
						std::cout << k << " position: " << P[0] << "-" << P[1] << "-" << P[2] << endl;
						Vec3f res = T * P;
						std::cout << k << " triangulation: " << res[0] << " - " << res[1] << " - " << res[2] << endl;
						float crossval = dot(P - positions[pos_a], cross(e1, e2));
						std::cout << "Crossval: " << crossval << endl;
						if (res[0] >= 0 && res[1] >= 0 && res[2] >= 0 && res[1] < 1 && res[2] < 1 && res[0] + res[1] + res[2] == 1) {
							std::cout << k << " is inside triangle." << endl;
							no_points_inside = false;
							break;
						}
						std::cout << k << " is not inside triangle." << endl;
					}

					// IF NO POINT WAS FOUND INSIDE THE TRIANGLE, ADD IT TO TRIANGULATION AND REPEAT
					if (no_points_inside) {
						f[0] = pos_a; f[2] = pos_b; f[4] = pos_c;

						// NORMAL DEFINITION
						Vec3f normal = cross(e1, e2);
						normal.normalize();

						normals.push_back(normal);

						f[1] = normal_number; f[3] = normal_number; f[5] = normal_number;
						normal_number += 1;
						faces.push_back(f);

						// EXTRACT EAR POINT FROM TEMP VECTOR AND BREAK
						temp.erase(temp.begin() + index);
						not_found = false;
					}
				}
			}
		}
	}
	common_ctrl_.message(("Loaded mesh from " + filename).c_str());
	return unpackIndexedData(positions, normals, faces);
}

void FW::init(void) {
	new App;
}
