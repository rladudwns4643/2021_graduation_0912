#pragma once
#include "UserInterface.h"

class Button : public UserInterface
{
public:
	explicit Button(std::string type, std::string id);
	virtual ~Button();
};

