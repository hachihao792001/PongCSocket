#include "Physics.h"

bool CheckCollide(MyRectangle rec1, MyRectangle rec2) {

	// Nếu 1 hình chữ nhật nằm phía bên trái hình chữ nhật kia 
	if (rec1.topLeftCorner.x >= rec2.topLeftCorner.x + rec2.width ||
		rec2.topLeftCorner.x >= rec1.topLeftCorner.x + rec1.width)
		return false;

	// Nếu 1 hình chữ nhật nằm phía bên trên hình chữ nhật kia
	if (rec1.topLeftCorner.y >= rec2.topLeftCorner.y + rec2.height ||
		rec2.topLeftCorner.y >= rec1.topLeftCorner.y + rec1.height)
		return false;
	return true;
}