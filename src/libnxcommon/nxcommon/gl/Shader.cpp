/*
	Copyright 2010-2015 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Shader.h"
#include "GLException.h"
#include "gl.h"
#include "../file/FileException.h"
#include <cstring>
#include <cstdio>
#include <string>
#include <regex>

using std::streamoff;
using std::string;
using std::regex;
using std::smatch;



Shader::Shader()
		: type(GL_NONE)
{
}


Shader::Shader(GLenum type, const CString& name)
		: type(type),
#ifdef GTA_USE_OPENGL_ES
		  shader(0),
#else
		  shader(glCreateShader(type)),
#endif
		  name(name), resolveIncludesEnabled(false)
{
}


Shader::~Shader()
{
	glDeleteShader(shader);
}


void Shader::setResolveIncludes(bool resolve)
{
	this->resolveIncludesEnabled = resolve;
}


void Shader::setIncludeRoot(const File& root)
{
	this->includeRoot = root;
}


CString Shader::resolveIncludes(const CString& code)
{
	string content(code.get(), code.length());
	CString newContent;

	regex incRegex("#[[:space:]]*include[[:space:]]+\"([^\"]+)\"");

	smatch match;
	while (regex_search(content, match, incRegex)) {
		newContent.append(match.prefix().str().c_str());

		CString relIncFile(match[1].str().c_str());
		File incFile;

		if (!includeRoot.isNull()) {
			incFile = File(includeRoot, relIncFile);
		} else {
			incFile = File(relIncFile);
		}

		if (!incFile.isRegularFile()) {
			throw FileException (
					CString("Include file ").append(relIncFile).append(" doesn't exist or is not a regular file!"),
					__FILE__, __LINE__);
		}

		CString incContent(incFile.readAll());
		CString resolvedIncContent = resolveIncludes(incContent);
		newContent.append(resolvedIncContent);

		content = match.suffix().str();
	}

	newContent.append(content.c_str());

	return newContent;
}


void Shader::loadSourceCode(const CString& code)
{
	CString resolvedCode = code;

	if (resolveIncludesEnabled) {
		resolvedCode = resolveIncludes(code);
	}

#ifdef GTA_USE_OPENGL_ES
	this->code = resolvedCode;
#else
	GLint len = resolvedCode.length();
	const char* ccode = resolvedCode.get();
	glShaderSource(shader, 1, &ccode, &len);
#endif
}


void Shader::loadSourceCode(const File& file)
{
	loadSourceCode(CString(file.readAll()));
}


#ifdef GTA_USE_OPENGL_ES
void Shader::compile() {}
#endif


#ifdef GTA_USE_OPENGL_ES
void Shader::glesForceCompile()
#else
void Shader::compile()
#endif
{
#ifdef GTA_USE_OPENGL_ES
	if (shader == 0) {
		shader = glCreateShader(type);
	}

	CString newCode("#version 100\n");
	newCode.append(code);

	GLint len = newCode.length();
	const char* ccode = newCode.get();
	glShaderSource(shader, 1, &ccode, &len);
#endif

	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		char* log = new char[maxLength];
		glGetShaderInfoLog(shader, maxLength, &actualLength, log);

		size_t nameLen = !name.isNull() ? name.length() : 16;
		char* errmsg = new char[actualLength + nameLen + 64];
		sprintf(errmsg, "Error compiling shader \"%s\" [#%d]. Info log:\n\n%s",
				!name.isNull() ? name.get() : "[UNNAMED]", shader, log);
		delete[] log;
		GLException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	} else {
		GLint maxLength;
		GLint actualLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		
		if (maxLength != 0) {
			char* log = new char[maxLength];
			glGetShaderInfoLog(shader, maxLength, &actualLength, log);

			CString clog = CString::from(log);
			clog.trim();

			if (clog.length() != 0) {
				printf("\nSuccessfully compiled shader \"%s\" [#%d]. Build log:\n==========\n%s\n==========\n\n",
						!name.isNull() ? name.get() : "[UNNAMED]", shader, clog.get());
			} else {
				printf("Successfully compiled shader \"%s\" [#%d]. Build log is empty\n",
						!name.isNull() ? name.get() : "[UNNAMED]", shader);
			}
		} else {
			printf("\nSuccessfully compiled shader \"%s\" [#%d]. Build log is empty\n\n",
					!name.isNull() ? name.get() : "[UNNAMED]", shader);
		}
	}
}
