/*

Copyright (c) 2005-2016, University of Oxford.
All rights reserved.

University of Oxford means the Chancellor, Masters and Scholars of the
University of Oxford, having an administrative office at Wellington
Square, Oxford OX1 2JD, UK.

This file is part of Chaste.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of the University of Oxford nor the names of its
   contributors may be used to endorse or promote products derived from this
   software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef ABSTRACTPDEMODIFIER_HPP_
#define ABSTRACTPDEMODIFIER_HPP_

#include "ChasteSerialization.hpp"
#include "ClassIsAbstract.hpp"
#include <boost/shared_ptr.hpp>

#include "AbstractCellBasedSimulationModifier.hpp"
#include "PdeAndBoundaryConditions.hpp"
#include "TetrahedralMesh.hpp"

/**
 * A modifier class in which has the common functionality of solving a PDE on an arbitrary mesh.
 * The results are stored in CellData.
 *
 * \todo Improve documentation (#2687)
 */
template<unsigned DIM>
class AbstractPdeModifier : public AbstractCellBasedSimulationModifier<DIM,DIM>
{
private:

    /** Needed for serialization. */
    friend class boost::serialization::access;
    /**
     * Boost Serialization method for archiving/checkpointing.
     * Archives the object and its member variables.
     *
     * @param archive  The boost archive.
     * @param version  The current version of this class.
     */
    template<class Archive>
    void serialize(Archive & archive, const unsigned int version)
    {
        archive & boost::serialization::base_object<AbstractCellBasedSimulationModifier<DIM,DIM> >(*this);
        archive & mpPdeAndBcs;

        // Note that archiving of mSolution is ~~~handled by the methods save/load_construct_data~~~ NOT YET DONE
        // archive & mpFeMesh;
        archive & mOutputDirectory;
        archive & mOutputGradient;
        archive & mOutputSolutionAtPdeNodes;
    }

protected:

    /** Shared pointer to a PDE object with associated boundary conditions. */
    boost::shared_ptr<PdeAndBoundaryConditions<DIM> > mpPdeAndBcs;

    /** The solution to the PDE problem at the current time step. */
    Vec mSolution; ///\todo NEED TO ARCHIVE THIS see AbstractPdeandBoundaryCondition (#2687)

    /** Pointer to the finite element mesh on which to solve the PDE. **/
    TetrahedralMesh<DIM,DIM>* mpFeMesh;  ///\todo #2687 NEED TO ARCHIVE THIS

    /** Store the output directory name. */
    std::string mOutputDirectory;

    /** Whether or not to calculate and output the gradient of the solution. */
    bool mOutputGradient;

    /**
     * Whether to output the PDE solution at each node of the FE mesh at output time steps.
     * Defaults to false.
     */
    bool mOutputSolutionAtPdeNodes;

    /** File that the values of the PDE solution are written out to. */
    out_stream mpVizPdeSolutionResultsFile;

public:

    /**
     * Constructor.
     *
     * @param pPdeAndBcs a shared pointer to a PDE object with associated boundary conditions
     */
    AbstractPdeModifier(boost::shared_ptr<PdeAndBoundaryConditions<DIM> > pPdeAndBcs=boost::shared_ptr<PdeAndBoundaryConditions<DIM> >());

    /**
     * Destructor.
     */
    virtual ~AbstractPdeModifier();

    /**
     * @return mpPdeAndBcs.
     */
    const boost::shared_ptr<PdeAndBoundaryConditions<DIM> > GetPdeAndBcs() const;

    /**
     * @return mSolution.
     */
    Vec GetSolution();

    /**
     * @return mpFeMesh.
     */
    TetrahedralMesh<DIM,DIM>* GetFeMesh() const;

    /**
     * Overridden SetupSolve() method.
     * \todo improve documentation (#2687)
     *
     * Specifies what to do in the simulation before the start of the time loop. Needs overwriting in child classes
     *
     * @param rCellPopulation reference to the cell population
     * @param outputDirectory the output directory, relative to where Chaste output is stored
     */
    virtual void SetupSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation, std::string outputDirectory);

    /**
     * UpdateAtEndOfTimeStep() method. Needs overwriting in child classes.
     * \todo improve documentation (#2687)
     *
     * Specifies what to do in the simulation at the end of each time step.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void UpdateAtEndOfTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation)=0;

    /**
     * Overridden UpdateAtEndOfOutputTimeStep() method.
     * \todo improve documentation (#2687)
     *
     * Specifies what to do in the simulation at the end of each output timestep.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void UpdateAtEndOfOutputTimeStep(AbstractCellPopulation<DIM,DIM>& rCellPopulation);

    /**
     * Overridden UpdateAtEndOfSolve() method.
     * \todo improve documentation (#2687)
     * Specify what to do in the simulation at the end of each time loop.
     *
     * @param rCellPopulation reference to the cell population
     */
    virtual void UpdateAtEndOfSolve(AbstractCellPopulation<DIM,DIM>& rCellPopulation);

    /**
     * Set whether to calculate and save the gradient of the solution to CellData.
     *
     * @return mOutputGradient
     */
    bool GetOutputGradient();

    /**
     * Set whether to calculate and save the gradient of the solution to CellData.
     *
     * @param outputGradient whether to output the gradient
     */
    void SetOutputGradient(bool outputGradient);

    /**
     * Set mOutputSolutionAtPdeNodes.
     *
     * @param outputSolutionAtPdeNodes whether to output the PDE solution at each node of the FE mesh at output time steps
     */
    void SetOutputSolutionAtPdeNodes(bool outputSolutionAtPdeNodes);

    /**
     * Overridden OutputSimulationModifierParameters() method.
     *
     * Output any simulation modifier parameters to file.
     *
     * @param rParamsFile the file stream to which the parameters are output
     */
    void OutputSimulationModifierParameters(out_stream& rParamsFile);
};

#include "SerializationExportWrapper.hpp"
TEMPLATED_CLASS_IS_ABSTRACT_1_UNSIGNED(AbstractPdeModifier)

#endif /*ABSTRACTPDEMODIFIER_HPP_*/
