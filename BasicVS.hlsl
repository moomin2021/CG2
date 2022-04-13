float4 main(float4 pos : POSITION) : SV_POSITION
{

	// ägèk
	//return pos * float4(2.0f, 2.0f, 1.0f, 1.0f);

	// âÒì]
	//float4 pos2 = pos;
	//const float PI = 3.14159;
	//float angle = PI * 0.25f;
	//pos2.x = pos.x * cos(angle) - pos.y * sin(angle);
	//pos2.y = pos.x * sin(angle) + pos.y * cos(angle);
	//return pos2;

	return pos;
}