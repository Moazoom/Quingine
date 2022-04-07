#ifndef BUFFERS_H
#define BUFFER_H

#include "glad.h"

// vertex buffer object class, model data goes in here
class VBO{
    public:
        GLuint ID;

        VBO(GLfloat* vertices, GLsizeiptr size){
            glGenBuffers(1, &ID);
            glBindBuffer(GL_ARRAY_BUFFER, ID);
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        }

        void Bind(){
            glBindBuffer(GL_ARRAY_BUFFER, ID);
        }
        void Unbind(){
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        void Delete(){
            glDeleteBuffers(1, &ID);
        }
};

// element buffer object, this keeps our indicies for faces
class EBO{
    public:
        GLuint ID;

        EBO(GLint* indices, GLsizeiptr size){
            glGenBuffers(1, &ID);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
        }

        void Bind(){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        }
        void Unbind(){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        void Delete(){
            glDeleteBuffers(1, &ID);
        }
};

//vertex array class, holds our vbo, ebo and any vertex attributes. neat
class VAO{
    public:
        GLuint ID;
        VAO(){
            glGenVertexArrays(1, &ID);
        }

        // links to our VBO and attributes and things
        void LinkVBO(VBO VBO, GLuint layout, int amount, int stride, int startpos){
            VBO.Bind();
            // this is designed only for floats, be carefull!
            glVertexAttribPointer(layout, amount, GL_FLOAT, GL_FALSE, stride * sizeof(GL_FLOAT), (void*)(startpos * sizeof(GL_FLOAT)));
            glEnableVertexAttribArray(layout);
            VBO.Unbind();
        }

        void Bind(){
            glBindVertexArray(ID);
        }
        void Unbind(){
            glBindVertexArray(0);
        }
        void Delete(){
            glDeleteVertexArrays(1, &ID);
        }
};


#endif