#   This file is part of TissueReconstruction.
#
# ** TissueReconstruction is free software: you can redistribute it and/or modify
# ** it under the terms of the GNU General Public License as published by
# ** the Free Software Foundation, either version 3 of the License, or
# ** (at your option) any later version.
# **
# ** TissueReconstruction is distributed in the hope that it will be useful,
# ** but WITHOUT ANY WARRANTY; without even the implied warranty of
# ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# ** GNU General Public License for more details.
# **
# ** You should have received a copy of the GNU General Public License
# ** along with TissueReconstruction.  If not, see <http://www.gnu.org/licenses/>.
# **
# */
 
DIRSRC_1		=	./src/
DIRSRC_2		=	./core/

SRCS		=	./main.c    \
                $(DIRSRC_1)process_image.c  \
                $(DIRSRC_1)restacking.c \
                $(DIRSRC_1)functions.c  \
                $(DIRSRC_2)area.c   \
                $(DIRSRC_2)close_area.c \
                $(DIRSRC_2)cross_correlation.c  \
                $(DIRSRC_2)edgelap.c    \
                $(DIRSRC_2)histogram.c  \
                $(DIRSRC_2)labeling.c   \
                $(DIRSRC_2)max_contrast.c   \
                $(DIRSRC_2)otsu_th.c    \
                $(DIRSRC_2)raw_files.c  \
                $(DIRSRC_2)reconstruction.c

INCLUDE		=	-I./header

LIB_PATH	=

RM		=	rm -Rf

LIBS	=	-lm

NAME	=	TissueReconstruction

FLAGS	=	-Wall -ggdb -std=c99 #-Wno-unused-result

CC		=	gcc

all:	compile

compile:
	@echo  -e "\n\tCompiling...\n"
	@$(CC) $(SRCS) -o $(NAME) $(INCLUDE) $(LIB_PATH) $(LIBS) $(FLAGS) `GraphicsMagick-config --cppflags --libs --ldflags`
        
clean:
	@echo  -e "\n\tCleaning...\n"
	@rm -rf $(NAME) $(NAME).dSYM $(OBJ)
