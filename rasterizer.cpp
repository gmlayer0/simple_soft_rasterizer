#include "vertex.h"
#include "utils.h"

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif

void vertex_transform(const glm::mat4 &transMatrix, const Vertex &in, Vertex &out) {
    out.texcoord = in.texcoord;
    out.position = transMatrix * in.position;
    // output should be in clip space.
}

bool in_side(int face_id, const glm::vec4 p) {
    switch (face_id) {
        case 0:
            return p[2] >= -p[3];
        case 1:
            return p[2] <= p[3];
        case 2:
            return p[0] >= -p[3];
        case 3:
            return p[0] <= p[3];
        case 4:
            return p[1] <= p[3];
        case 5:
            return p[1] >= -p[3];
        default:
            return false;
    }
}

Vertex intersect(int face_id, const Vertex &v1, const Vertex &v2) {
    float d1, d2;
    switch (face_id) {
        case 0:
            d1 = v1.position[2] + v1.position[3];
            d2 = v2.position[2] + v2.position[3];
            break;
        case 1:
            d1 = -v1.position[2] + v1.position[3];
            d2 = -v2.position[2] + v2.position[3];
            break;
        case 2:
            d1 = v1.position[0] + v1.position[3];
            d2 = v2.position[0] + v2.position[3];
            break;
        case 3:
            d1 = -v1.position[0] + v1.position[3];
            d2 = -v2.position[0] + v2.position[3];
            break;
        case 4:
            d1 = -v1.position[1] + v1.position[3];
            d2 = -v2.position[1] + v2.position[3];
            break;
        case 5:
            d1 = v1.position[1] + v1.position[3];
            d2 = v2.position[1] + v2.position[3];
            break;
        default:
            d1 = 1.f;
            d2 = 0.f;
            break;
    }

    float weight = d1 / (d1 - d2);
    Vertex ret{lerp(v1.position, v2.position, weight), lerp(v1.texcoord, v2.texcoord, weight)};
    return ret;
}

// return value: number of triangle generated.
int triangle_clip(const Vertex input[3], Vertex output[]) {
    // input and output should be in clip space.
    int buf_cnt = 0;
    Vertex buf[18];
    int output_cnt = 3;
    output[0] = input[0];
    output[1] = input[1];
    output[2] = input[2];
    for (int i = 0; i < 6; i++) {
        buf_cnt = 0;
        for (int j = 0; j < output_cnt; j++) {
            buf[buf_cnt++] = output[j];
        }
        output_cnt = 0;
        Vertex last = buf[buf_cnt - 1];
        bool last_in_side = in_side(i, last.position);
        for (int j = 0; j < buf_cnt; j++) {
            Vertex cur = buf[j];
            bool cur_in_side = in_side(i, cur.position);
            if (cur_in_side) {
                if (!last_in_side) {
                    output[output_cnt++] = intersect(i, last, cur);
                }
                output[output_cnt++] = cur;
            } else if (last_in_side) {
                output[output_cnt++] = intersect(i, last, cur);
            }
            last = cur;
            last_in_side = cur_in_side;
        }
    }
    return output_cnt;
}

void perspective_division(const Vertex &input, Vertex &output) {
    // 经过 clip，已经不存在位于相机原点的点了。
    for (int i = 0; i < 3; i++) {
        output.position[i] = input.position[i] / input.position[3];
    }
    // 输出已位于 NDC。
}

void screen_transform(const Vertex &input, Vertex &out, float width, float height) {
    // input here should be in NDC
    out.texcoord = input.texcoord;
    out.position[0] = (input.position[0] * width + width) / 2;
    out.position[1] = (input.position[1] * height + height) / 2;
    // output here should be in screen space
}

void half_space_rasterizer(const Vertex input[3], unsigned int width, unsigned int height, const char *tex,
                           unsigned int *fb) {
    // predeclear : input here should be in screen space
    // 0 - 1023.0f

    // convert parameters into fixed-points.
    // signed 11.5 -1024 - 1023.
    short max_x, max_y, min_x, min_y;
    short x[3], y[3], d[3];
    unsigned short u[3], v[3];
    for (int i = 0; i < 3; i++) {
        x[i] = (short) lroundf(32.0f * input[i].position[0]);
        y[i] = (short) lroundf(32.0f * input[i].position[1]);
        d[i] = (short) lroundf(32.0f * input[i].position[2]);
        u[i] = (unsigned short) (input[i].texcoord[0] * 65535.f);
        v[i] = (unsigned short) (input[i].texcoord[1] * 65535.f);
    }
    max_x = max(x[0], max(x[1], x[2]));
    min_x = min(x[0], min(x[1], x[2]));
    max_y = max(y[0], max(y[1], y[2]));
    min_y = min(y[0], min(y[1], y[2]));
    // signed 11.5 -1024 - 1023.
    short DF01DX, DF12DX, DF20DX, DF01DY, DF12DY, DF20DY;
    // signed 27.5
    int F01_0, F12_0, F20_0;
    DF01DX = (short) (y[0] - y[1]);
    DF12DX = (short) (y[1] - y[2]);
    DF20DX = (short) (y[2] - y[0]);
    DF01DY = (short) (x[1] - x[0]);
    DF12DY = (short) (x[2] - x[1]);
    DF20DY = (short) (x[0] - x[2]);
    F01_0 = ((x[0] * y[1]) >> 5) - ((x[1] * y[0]) >> 5);
    F12_0 = ((x[1] * y[2]) >> 5) - ((x[2] * y[1]) >> 5);
    F20_0 = ((x[2] * y[0]) >> 5) - ((x[0] * y[2]) >> 5);
    int delta = F01_0 + F12_0 + F20_0;
    if (delta <= 0) return;
    int F01_y, F12_y, F20_y;
    F01_y = F01_0 + ((DF01DX * min_x) >> 5) + ((DF01DY * min_y) >> 5);
    F12_y = F12_0 + ((DF12DX * min_x) >> 5) + ((DF12DY * min_y) >> 5);
    F20_y = F20_0 + ((DF20DX * min_x) >> 5) + ((DF20DY * min_y) >> 5);
    // todo insert uv.
    float invert_delta = 1.0f / (float) delta;
    for (signed short iy = min_y; iy < max_y; iy += (1 << 5)) {
        int F01_x = F01_y;
        int F12_x = F12_y;
        int F20_x = F20_y;
        for (signed short ix = min_x; ix < max_x; ix += (1 << 5)) {
            if (((F01_x | F12_x | F20_x) & 0x80000000) == 0) {
                fb[(iy >> 5) * width + (ix >> 5)] = 0xffffffff;
            } else {
                fb[(iy >> 5) * width + (ix >> 5)] = 0xff000000;
            }
            F01_x = (F01_x + DF01DX);
            F12_x = (F12_x + DF12DX);
            F20_x = (F20_x + DF20DX);
        }
        F01_y = (F01_y + DF01DY);
        F12_y = (F12_y + DF12DY);
        F20_y = (F20_y + DF20DY);
    }
}
