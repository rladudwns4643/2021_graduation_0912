#pragma once
#include "UserInterface.h"

class ImageView : public UserInterface
{
public:
	explicit ImageView(std::string type, std::string id);
	virtual ~ImageView();
};

