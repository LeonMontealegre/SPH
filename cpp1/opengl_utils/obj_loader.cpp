#include "obj_loader.hpp"

using namespace std;

struct reader2 : ctype<char> {
    reader2() : ctype<char>(get_table()) {}
    static ctype_base::mask const* get_table() {
        static vector<ctype_base::mask> rc(table_size, ctype_base::mask());
        rc['\n'] = ctype_base::space;
        rc[' '] = ctype_base::space;
        rc['/'] = ctype_base::space;
        return &rc[0];
    }
};

struct index_group {
    const int NO_VALUE = -1;
    int pos, tc, norm;
    index_group() : pos(NO_VALUE), tc(NO_VALUE), norm(NO_VALUE) {}
    void init(int p, int t, int n) {
        pos = p-1;
        tc = t-1;
        norm = n-1;
    }
};

struct face {
    index_group groups[3];
    
    face(int v1, int t1, int n1, int v2, int t2, int n2, int v3, int t3, int n3) {
        groups[0].init(v1, t1, n1);
        groups[1].init(v2, t2, n2);
        groups[2].init(v3, t3, n3);
    }
};

void processFaceVertex(index_group& indices, vector<float>& texts, vector<float>& norms, vector<int>& indicesList, float* textCoordArr, float* normArr) {
    
    int posIndex = indices.pos;
    indicesList.push_back(posIndex);
    
    textCoordArr[posIndex * 2] = texts[indices.tc * 2];
    textCoordArr[posIndex * 2 + 1] = 1 - texts[indices.tc * 2 + 1];
    
    normArr[posIndex * 3] = norms[indices.norm * 3];
    normArr[posIndex * 3 + 1] = norms[indices.norm * 3 + 1];
    normArr[posIndex * 3 + 2] = norms[indices.norm * 3 + 2];
}

mesh& reorderAttributes(vector<float>& verts, vector<float>& texts, vector<float>& norms, vector<face>& faces) {
    vector<int> indices;
    vector<int> vertsIndices;
    vector<int> normsIndices;
    vector<int> textsIndices;
    
    for (int f = 0; f < faces.size(); ++f) {
        for (int g = 0; g < 3; ++g) {
            int v = 0;
            bool create = true;
            for (; v < vertsIndices.size(); ++v) {
                if (vertsIndices[v] == faces[f].groups[g].pos && normsIndices[v] == faces[f].groups[g].norm && textsIndices[v] == faces[f].groups[g].tc) {
                    create = false;
                    break;
                }
            }
            if (create) {
                vertsIndices.push_back(faces[f].groups[g].pos);
                normsIndices.push_back(faces[f].groups[g].norm);
                textsIndices.push_back(faces[f].groups[g].tc);
            }
            indices.push_back(v);
        }
    }
    
    unsigned int indicesArr[indices.size()];
    for (int i = 0; i < indices.size(); ++i) {
        indicesArr[i] = (unsigned int)indices[i];
    }
    
    float posArr[vertsIndices.size()*3];
    for (int i = 0; i < vertsIndices.size(); ++i) {
        posArr[3*i+0] = verts[3*vertsIndices[i]+0];
        posArr[3*i+1] = verts[3*vertsIndices[i]+1];
        posArr[3*i+2] = verts[3*vertsIndices[i]+2];
    }
    
    float normArr[normsIndices.size()*3];
    for (int i = 0; i < normsIndices.size(); ++i) {
        normArr[3*i+0] = norms[3*normsIndices[i]+0];
        normArr[3*i+1] = norms[3*normsIndices[i]+1];
        normArr[3*i+2] = norms[3*normsIndices[i]+2];
    }
    
    float texArr[textsIndices.size()*2];
    for (int i = 0; i < textsIndices.size(); ++i) {
        texArr[2*i+0] = texts[2*textsIndices[i]+0];
        texArr[2*i+1] = 1 - texts[2*textsIndices[i]+1];
    }
    
    cout << vertsIndices.size()*3 << endl;
    
    return *(new mesh(vertsIndices.size()*3, indices.size(), posArr, indicesArr, texArr, normArr));
}

mesh& loadOBJ(string file) {
    ifstream input(file);
    input.imbue(locale(locale(), new reader2()));
    
    
    vector<float> verts;
    vector<float> texts;
    vector<float> norms;
    vector<face> faces;
    
    string token;
    // For every line in the file
    for (;input >> token;) {
        if (token.compare("v") == 0) {
            float x, y, z;
            input >> x >> y >> z;
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
        } else if (token.compare("vt") == 0) {
            float u, v;
            input >> u >> v;
            texts.push_back(u);
            texts.push_back(v);
        } else if (token.compare("vn") == 0) {
            float x, y, z;
            input >> x >> y >> z;
            norms.push_back(x);
            norms.push_back(y);
            norms.push_back(z);
        } else if (token.compare("f") == 0) {
            int v1, t1, n1, v2, t2, n2, v3, t3, n3;
            input >> v1 >> t1 >> n1 >> v2 >> t2 >> n2 >> v3 >> t3 >> n3;
            face f(v1, t1, n1, v2, t2, n2, v3, t3, n3);
            faces.push_back(f);
        }
    }
    
    input.close();
    
    return reorderAttributes(verts, texts, norms, faces);
}
