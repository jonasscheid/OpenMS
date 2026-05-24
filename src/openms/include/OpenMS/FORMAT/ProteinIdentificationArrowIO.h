// Copyright (c) 2002-present, OpenMS Inc. -- EKU Tuebingen, ETH Zurich, and FU Berlin
// SPDX-License-Identifier: BSD-3-Clause
//
// --------------------------------------------------------------------------
// $Maintainer: Timo Sachsenberg $
// $Authors: Timo Sachsenberg $
// --------------------------------------------------------------------------

#pragma once

#include <OpenMS/config.h>

#include <OpenMS/CONCEPT/Types.h>
#include <OpenMS/METADATA/ProteinIdentification.h>
#include <OpenMS/METADATA/PeptideIdentificationList.h>
#include <OpenMS/FORMAT/MSExperimentArrowExport.h>

#include <map>
#include <memory>
#include <vector>

// Forward declarations
namespace arrow
{
  class Table;
}

namespace OpenMS
{

/**
  @brief Import and export ProteinIdentification data to/from Apache Arrow format

  This class provides static methods to export and import ProteinIdentification
  data to/from Apache Arrow Tables and Parquet files. Separate tables are
  provided for protein hits, protein groups, and search parameters.

  @experimental This API is experimental and may change in future versions.

  @ingroup FileIO
*/
class OPENMS_DLLAPI ProteinIdentificationArrowIO
{
public:
  // ==================== Export methods ====================

  /**
    @brief Export protein hits to Apache Arrow Table

    Each ProteinHit becomes one row with identification, score, and metadata.

    @param[in] protein_identifications Vector of protein identifications
    @return Shared pointer to Arrow Table, or nullptr on error
  */
  static std::shared_ptr<arrow::Table> exportProteinsToArrow(
    const std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Export protein hits to Parquet file

    @param[in] protein_identifications Vector of protein identifications
    @param[in] filename Output file path
    @param[in] config Parquet writing options
    @return true on success, false on error
  */
  static bool exportProteinsToParquet(
    const std::vector<ProteinIdentification>& protein_identifications,
    const String& filename,
    const ParquetWriteConfig& config = ParquetWriteConfig{});

  /**
    @brief Export protein groups to Apache Arrow Table

    Each ProteinGroup becomes one row with group probability and member accessions.

    @param[in] protein_identifications Vector of protein identifications
    @return Shared pointer to Arrow Table, or nullptr on error
  */
  static std::shared_ptr<arrow::Table> exportProteinGroupsToArrow(
    const std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Export protein groups to Parquet file

    @param[in] protein_identifications Vector of protein identifications
    @param[in] filename Output file path
    @param[in] config Parquet writing options
    @return true on success, false on error
  */
  static bool exportProteinGroupsToParquet(
    const std::vector<ProteinIdentification>& protein_identifications,
    const String& filename,
    const ParquetWriteConfig& config = ParquetWriteConfig{});

  /**
    @brief Export search parameters to Apache Arrow Table

    Each ProteinIdentification's SearchParameters becomes one row.

    @param[in] protein_identifications Vector of protein identifications
    @return Shared pointer to Arrow Table, or nullptr on error
  */
  static std::shared_ptr<arrow::Table> exportSearchParamsToArrow(
    const std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Export search parameters to Parquet file

    @param[in] protein_identifications Vector of protein identifications
    @param[in] filename Output file path
    @param[in] config Parquet writing options
    @return true on success, false on error
  */
  static bool exportSearchParamsToParquet(
    const std::vector<ProteinIdentification>& protein_identifications,
    const String& filename,
    const ParquetWriteConfig& config = ParquetWriteConfig{});

  // ==================== Import methods ====================

  /**
    @brief Import all three Parquet files and reconstruct ProteinIdentifications

    Reads the three Parquet files and reconstructs a vector of
    ProteinIdentification objects with hits, groups, and search parameters.

    @param[in] proteins_filename Path to proteins Parquet file
    @param[in] protein_groups_filename Path to protein groups Parquet file
    @param[in] search_params_filename Path to search params Parquet file
    @param[out] protein_identifications Reconstructed protein identifications
    @return true on success, false on error
  */
  static bool importFromParquet(
    const String& proteins_filename,
    const String& protein_groups_filename,
    const String& search_params_filename,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import search parameters from Arrow Table

    Each row becomes a ProteinIdentification shell with run-level metadata
    and SearchParameters populated.

    @param[in] table Arrow Table with search parameters
    @param[out] protein_identifications Reconstructed protein identifications
    @return true on success, false on error
  */
  static bool importSearchParamsFromArrow(
    const std::shared_ptr<arrow::Table>& table,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import protein hits from Arrow Table

    Adds ProteinHits to matching ProteinIdentifications by run_identifier.
    If no matching ProteinIdentification exists, creates new ones.

    @param[in] table Arrow Table with protein hits
    @param[out] protein_identifications Protein identifications to populate
    @return true on success, false on error
  */
  static bool importProteinsFromArrow(
    const std::shared_ptr<arrow::Table>& table,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import protein groups from Arrow Table

    Adds ProteinGroups and IndistinguishableProteins to matching
    ProteinIdentifications by run_identifier.

    @param[in] table Arrow Table with protein groups
    @param[out] protein_identifications Protein identifications to populate
    @return true on success, false on error
  */
  static bool importProteinGroupsFromArrow(
    const std::shared_ptr<arrow::Table>& table,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import search parameters from Parquet file

    @param[in] filename Path to Parquet file
    @param[out] protein_identifications Reconstructed protein identifications
    @return true on success, false on error
  */
  static bool importSearchParamsFromParquet(
    const String& filename,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import protein hits from Parquet file

    @param[in] filename Path to Parquet file
    @param[out] protein_identifications Protein identifications to populate
    @return true on success, false on error
  */
  static bool importProteinsFromParquet(
    const String& filename,
    std::vector<ProteinIdentification>& protein_identifications);

  /**
    @brief Import protein groups from Parquet file

    @param[in] filename Path to Parquet file
    @param[out] protein_identifications Protein identifications to populate
    @return true on success, false on error
  */
  static bool importProteinGroupsFromParquet(
    const String& filename,
    std::vector<ProteinIdentification>& protein_identifications);

  // ==================== Identifier handling parity with XML lane ====================

  /**
    @brief Synthesize `<search_engine>_<date>_<UniqueIdGenerator>` identifiers
           per ProtID, mirroring IdXMLFile.cpp:530.

    Public-static (not private) so the 3 outer parquet load entry points
    (PSMArrowIO / FeatureMapArrowIO / ConsensusMapArrowIO) can each invoke it
    after their own table loads complete. Returns the stored->synthesized map
    so callers can re-stamp every pep_id collection they own.

    Edge cases: empty search engine -> "unknown"; invalid date -> "1900-01-01T00:00:00";
    duplicate stored identifiers each get a distinct synth but the map collapses
    to last-seen (one ProtID is orphaned, WARN logged).
  */
  static std::map<String, String> synthesizeRunIdentifiers(
    std::vector<ProteinIdentification>& protein_identifications);

  /// Apply a synthesizeRunIdentifiers rename map to @p pep_ids. Pep_ids whose
  /// stored identifier isn't in the map are left untouched (matches XML lane).
  static void applyRunIdentifierRename(
    const std::map<String, String>& rename,
    PeptideIdentificationList& pep_ids);

  /**
    @brief Mirror of XMLHandler::checkUniqueIdentifiers_ — throws ParseError on duplicates.

    Public-static for the same reason as synthesizeRunIdentifiers. Called before
    any Arrow builder allocation in all 3 store entry points so a rejection
    leaves no partial file on disk. ParseError matches the XML lane's exception
    type and message text so log-grepping for the canonical text finds both lanes.
  */
  static void checkUniqueIdentifiers(
    const std::vector<ProteinIdentification>& protein_identifications);
};

} // namespace OpenMS
