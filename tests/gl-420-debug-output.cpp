//**********************************
// OpenGL Samples Pack 
// ogl-samples.g-truc.net
//**********************************
// OpenGL Debug Output
// 27/01/2012 - 27/01/2012
//**********************************
// Christophe Riccio
// ogl-samples@g-truc.net
//**********************************
// G-Truc Creation
// www.g-truc.net
//**********************************

#include <glf/glf.hpp>

namespace
{
	char const * SAMPLE_NAME("OpenGL Debug Output");
	char const * VERT_SHADER_SOURCE("gl-420/debug-output.vert");
	char const * FRAG_SHADER_SOURCE("gl-420/debug-output.frag");
	int const SAMPLE_SIZE_WIDTH(640);
	int const SAMPLE_SIZE_HEIGHT(480);
	int const SAMPLE_MAJOR_VERSION(4);
	int const SAMPLE_MINOR_VERSION(2);

	glf::window Window(glm::ivec2(SAMPLE_SIZE_WIDTH, SAMPLE_SIZE_HEIGHT));

	GLsizei const VertexCount(4);
	GLsizeiptr const PositionSize = VertexCount * sizeof(glm::vec2);
	glm::vec2 const PositionData[VertexCount] =
	{
		glm::vec2(-1.0f,-1.0f),
		glm::vec2( 1.0f,-1.0f),
		glm::vec2( 1.0f, 1.0f),
		glm::vec2(-1.0f, 1.0f)
	};

	GLsizei const ElementCount(6);
	GLsizeiptr const ElementSize = ElementCount * sizeof(GLushort);
	GLushort const ElementData[ElementCount] =
	{
		0, 1, 2, 
		2, 3, 0
	};

	GLuint const Instances(2);

	namespace buffer
	{
		enum type
		{
			ELEMENT,
			VERTEX,
			TRANSFORM,
			MATERIAL,
			MAX
		};
	}//namespace pipeline

	GLuint PipelineName(0);
	GLuint ProgramName(0);
	GLuint VertexArrayName(0);
	GLuint BufferName[buffer::MAX] = {0, 0, 0, 0};
	GLint UniformBufferOffset(0);
	GLint UniformBlockSize(0);
	GLint UniformInstance(0);
}//namespace

bool initProgram()
{
	bool Validated(true);
	
	glGenProgramPipelines(1, &PipelineName);
	glBindProgramPipeline(PipelineName);

	if(Validated)
	{
		glf::compiler Compiler;
		GLuint VertShaderName = Compiler.create(GL_VERTEX_SHADER, glf::DATA_DIRECTORY + VERT_SHADER_SOURCE, 
			"--version 420 --profile core");
		GLuint FragShaderName = Compiler.create(GL_FRAGMENT_SHADER, glf::DATA_DIRECTORY + FRAG_SHADER_SOURCE,
			"--version 420 --profile core");
		Validated = Validated && Compiler.check();

		ProgramName = glCreateProgram();
		glProgramParameteri(ProgramName, GL_PROGRAM_SEPARABLE, GL_TRUE);
		glAttachShader(ProgramName, VertShaderName);
		glAttachShader(ProgramName, FragShaderName);
		glLinkProgram(ProgramName);

		Validated = Validated && glf::checkProgram(ProgramName);
	}

	if(Validated)
		glUseProgramStages(PipelineName, GL_VERTEX_SHADER_BIT | GL_FRAGMENT_SHADER_BIT, ProgramName);

	if(Validated)
	{
		UniformInstance = glGetUniformLocation(ProgramName, "Instance");
	}

	glBindProgramPipeline(0);

	return Validated;
}

bool initVertexArray()
{
	bool Validated(true);

	glGenVertexArrays(1, &VertexArrayName);
	glBindVertexArray(VertexArrayName);
		glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
		glVertexAttribPointer(glf::semantic::attr::POSITION, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(glf::semantic::attr::POSITION);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
	glBindVertexArray(0);

	return Validated;
}

bool initBuffer()
{
	bool Validated(true);

	glGenBuffers(buffer::MAX, BufferName);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferName[buffer::ELEMENT]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ElementSize, ElementData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, BufferName[buffer::VERTEX]);
	glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	{
		glGetActiveUniformBlockiv(
			ProgramName, 
			glf::semantic::uniform::TRANSFORM0,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			&UniformBlockSize);
		UniformBlockSize = glm::max(UniformBufferOffset, UniformBlockSize);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize * Instances, 0, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	{
		glm::vec4 Diffuse(1.0f, 0.5f, 0.0f, 1.0f);

		glGetActiveUniformBlockiv(
			ProgramName, 
			glf::semantic::uniform::MATERIAL,
			GL_UNIFORM_BLOCK_DATA_SIZE,
			&UniformBlockSize);

		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::MATERIAL]);
		glBufferData(GL_UNIFORM_BUFFER, UniformBlockSize, &Diffuse[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	return Validated;
}

bool initDebugOutput()
{
	bool Validated(true);

	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(&glf::debugOutput, NULL);
	GLuint MessageId(4);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_FALSE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_OTHER_ARB, GL_DONT_CARE, 0, NULL, GL_TRUE);
	glDebugMessageControlARB(GL_DEBUG_SOURCE_APPLICATION_ARB, GL_DEBUG_TYPE_OTHER_ARB, GL_DONT_CARE, 1, &MessageId, GL_FALSE);
	std::string Message1("Message 1");
	glDebugMessageInsertARB(
		GL_DEBUG_SOURCE_APPLICATION_ARB, 
		GL_DEBUG_TYPE_OTHER_ARB, 1, 
		GL_DEBUG_SEVERITY_MEDIUM_ARB,
		GLsizei(Message1.size()), Message1.c_str());
	std::string Message2("Message 2");
	glDebugMessageInsertARB(
		GL_DEBUG_SOURCE_THIRD_PARTY_ARB, 
		GL_DEBUG_TYPE_OTHER_ARB, 2, 
		GL_DEBUG_SEVERITY_MEDIUM_ARB,
		GLsizei(Message2.size()), Message2.c_str());
	glDebugMessageInsertARB(
		GL_DEBUG_SOURCE_APPLICATION_ARB, 
		GL_DEBUG_TYPE_OTHER_ARB, 2, 
		GL_DEBUG_SEVERITY_MEDIUM_ARB,
		-1, "Message 3");
	glDebugMessageInsertARB(
		GL_DEBUG_SOURCE_APPLICATION_ARB, 
		GL_DEBUG_TYPE_OTHER_ARB, MessageId, 
		GL_DEBUG_SEVERITY_MEDIUM_ARB,
		-1, "Message 4");

	return Validated;
}

bool begin()
{
	bool Validated = glf::checkGLVersion(SAMPLE_MAJOR_VERSION, SAMPLE_MINOR_VERSION);

	glGetIntegerv(
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		&UniformBufferOffset);

	if(Validated && glf::checkExtension("GL_ARB_debug_output"))
		Validated = initDebugOutput();
	if(Validated)
		Validated = initProgram();
	if(Validated)
		Validated = initBuffer();
	if(Validated)
		Validated = initVertexArray();

	return Validated;
}

bool end()
{
	bool Validated(true);

	glDeleteProgramPipelines(1, &PipelineName);
	glDeleteVertexArrays(1, &VertexArrayName);
	glDeleteBuffers(buffer::MAX, BufferName);
	glDeleteProgram(ProgramName);

	return Validated;
}

void display()
{
	GLsizeiptr BufferSize = sizeof(glm::mat4);

	{
		glBindBuffer(GL_UNIFORM_BUFFER, BufferName[buffer::TRANSFORM]);

		glm::byte* Pointer = (glm::byte*)glMapBufferRange(
			GL_UNIFORM_BUFFER, 0, BufferSize,
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Window.TranlationCurrent.y));

		{
			glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y + 45.0f, glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x + 45.0f, glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;

			*(glm::mat4*)Pointer = MVP;
		}
		{
			glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Window.RotationCurrent.y + 45.0f, glm::vec3(1.f, 0.f, 0.f));
			glm::mat4 View = glm::rotate(ViewRotateX, Window.RotationCurrent.x + 90.f + 45.0f, glm::vec3(0.f, 1.f, 0.f));
			glm::mat4 Model = glm::mat4(1.0f);
			glm::mat4 MVP = Projection * View * Model;

			*(glm::mat4*)(Pointer + UniformBlockSize) = MVP;
		}

		// Make sure the uniform buffer is uploaded
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	glViewportIndexedf(0, 0, 0, float(Window.Size.x), float(Window.Size.y));
	glClearBufferfv(GL_UNIFORM_BUFFER, 0, &glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)[0]);

	glBindProgramPipeline(PipelineName);
	glBindVertexArray(VertexArrayName);
	glBindBufferRange(GL_UNIFORM_BUFFER, glf::semantic::uniform::TRANSFORM0, BufferName[buffer::TRANSFORM], 0, BufferSize);
	glBindBufferRange(GL_UNIFORM_BUFFER, glf::semantic::uniform::TRANSFORM1, BufferName[buffer::TRANSFORM], UniformBlockSize, BufferSize);
	glBindBufferBase(GL_TEXTURE_BUFFER, glf::semantic::uniform::MATERIAL, BufferName[buffer::MATERIAL]);

	for(GLint i = 0; i < 2; ++i)
	{
		glProgramUniform1i(ProgramName, UniformInstance, i);
		glDrawElementsInstancedBaseVertexBaseInstance(
			GL_QUADS, ElementCount, GL_FLOAT, 0, 1, 0,-1);
	}

	glf::swapBuffers();
}

int main(int argc, char* argv[])
{
	return glf::run(
		argc, argv,
		glm::ivec2(::SAMPLE_SIZE_WIDTH, ::SAMPLE_SIZE_HEIGHT), 
		glf::CORE,
		::SAMPLE_MAJOR_VERSION, 
		::SAMPLE_MINOR_VERSION);
}
